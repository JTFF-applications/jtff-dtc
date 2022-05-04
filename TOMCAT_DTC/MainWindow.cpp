#include <QFileDialog>
#include <QMessageBox>

#include <fstream>

#include <pugixml.hpp>

#include <Windows/WaypointAddDialog.h>
#include <Utilities/CombatFlite.h>

#include "Navgrid.h"
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), m_logger(Logger::Get()), m_connector("127.0.0.1", 7778)
{
	m_ui.setupUi(this);

	m_availableWaypoints = QList<QString>::fromVector({ "WP1", "WP2", "WP3", "FP", "IP", "HB", "DP", "HA" });

	QObject::connect(m_ui.add_wpt_btn, &QPushButton::clicked, this, &MainWindow::on_add_wpt_clicked);
	QObject::connect(m_ui.rm_wpt_btn, &QPushButton::clicked, this, &MainWindow::on_rm_wpt_clicked);
	QObject::connect(m_ui.modify_wpy_btn, &QPushButton::clicked, this, &MainWindow::on_modify_wpt_clicked);

	QObject::connect(m_ui.navgrid_validate_btn, &QPushButton::clicked, this, &MainWindow::on_navgrid_validate_clicked);
	QObject::connect(m_ui.navgrid_lat, &QLineEdit::textChanged, this, &MainWindow::on_navgrid_coordinates_changed);
	QObject::connect(m_ui.navgrid_lon, &QLineEdit::textChanged, this, &MainWindow::on_navgrid_coordinates_changed);

	QObject::connect(m_ui.import_file_btn, &QPushButton::clicked, this, &MainWindow::on_import_file_clicked);
	QObject::connect(m_ui.export_file_btn, &QPushButton::clicked, this, &MainWindow::on_export_file_clicked);
	QObject::connect(m_ui.import_cf_btn, &QPushButton::clicked, this, &MainWindow::on_import_cf_clicked);
	QObject::connect(m_ui.export_ac_btn, &QPushButton::clicked, this, &MainWindow::on_export_ac_clicked);
}

void MainWindow::on_add_wpt_clicked()
{
	if (m_waypoints.size() + 1 > m_maxWaypoints)
	{
		QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", "You can't enter more waypoint for the tomcat !", QMessageBox::Ok).exec();
		return;
	}

	WaypointAddDialog wad(m_availableWaypoints);
	wad.SetFunctions([&] {
		auto data = wad.GetData();
		if (data.first.empty())
			return;
		AddWaypoint(data);
		wad.close();
		},
		[&] {
			wad.close();
		});
	wad.exec();
}

void MainWindow::on_rm_wpt_clicked()
{
	for (const auto& item : m_ui.wpt_list->selectedItems())
	{
		RemoveWaypoint(FindWaypoint(item->text().toStdString()));
	}
}

void MainWindow::on_modify_wpt_clicked()
{
	for (const auto& item : m_ui.wpt_list->selectedItems())
	{
		const auto& wpt = FindWaypoint(item->text().toStdString());
		m_availableWaypoints.append(wpt.first.c_str());

		WaypointAddDialog wad(m_availableWaypoints);
		wad.SetData(wpt.second);
		wad.SetCurrentWpt(wpt.first);
		wad.SetFunctions([&] {
			const auto& data = wad.GetData();
			if (data.first.empty())
				return;
			RemoveWaypoint(wpt);
			AddWaypoint(data);
			wad.close();
			}, [&] {
				m_availableWaypoints.removeAll(wpt.first.c_str());
				wad.close();
			});
		wad.exec();
	}
}

void MainWindow::on_navgrid_validate_clicked()
{
	auto ttot = m_ui.navgrid_validate_btn->styleSheet().toStdString();
	try
	{
		if (Coordinate::Check(m_ui.navgrid_lat->text().toStdString()) && Coordinate::Check(m_ui.navgrid_lon->text().toStdString()))
			m_ui.navgrid_validate_label->setStyleSheet("background-color: green; border-radius: 4px;");
		else
			m_ui.navgrid_validate_label->setStyleSheet("background-color: red; border-radius: 4px;");
	}
	catch (const Coordinate::bad_coordinate& ex)
	{
		m_ui.navgrid_validate_label->setStyleSheet("background-color: red; border-radius: 4px;");
	}
}

void MainWindow::on_navgrid_coordinates_changed()
{
	m_ui.navgrid_validate_label->setStyleSheet("");
}

void MainWindow::on_import_file_clicked()
{
	std::string fileName = QFileDialog::getOpenFileName(this, "Import Waypoints", "", "XML File (*.xml)").toStdString();
	if (fileName.empty() && std::filesystem::exists(fileName))
		return;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str(), pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", "Failed to parse XML file !", QMessageBox::Ok).exec();
		m_logger->error(std::format("Failed to open selected file : {} ! Aborting...", fileName));
		return;
	}

	if (std::string(doc.document_element().attribute("name").value()) != "F14")
	{
		QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", "This file is not for the F14 !", QMessageBox::Ok).exec();
		m_logger->error("This file is not for the tomcat ! You can't open it !");
		return;
	}

	// Navgrid
	pugi::xpath_node navgrid = doc.select_single_node("/Aircraft/Navgrid");
	if (navgrid)
	{
		m_ui.navgrid_btn->setChecked(navgrid.node().attribute("enabled").as_bool());
		m_ui.navgrid_lat->setText(navgrid.node().attribute("lattitude").as_string());
		m_ui.navgrid_lon->setText(navgrid.node().attribute("longitude").as_string());
		m_ui.navgrid_bearing->setValue(navgrid.node().attribute("bearing").as_int());
		m_ui.navgrid_width->setValue(navgrid.node().attribute("width").as_int());
		m_ui.navgrid_sectors->setValue(navgrid.node().attribute("sectors").as_int());
	}

	// Waypoints
	pugi::xpath_node_set wpts = doc.select_nodes("/Aircraft/Waypoints/Waypoint");
	for (const auto& wpt : wpts)
	{
		std::string type = wpt.node().attribute("type").as_string();
		std::string lat = wpt.node().attribute("lattitude").as_string();
		std::string lon = wpt.node().attribute("longitude").as_string();
		std::string name = wpt.node().attribute("name").as_string();
		int alt = wpt.node().attribute("altitude").as_int();
		int spd = wpt.node().attribute("speed").as_int();
		int hdg = wpt.node().attribute("heading").as_int();

		try
		{
			Waypoint wpt = Waypoint::FromStringCoordinates(lat, lon);
			wpt.SetName(name);
			wpt.SetAltitude(alt);
			wpt.SetSpeed(spd);
			wpt.SetHeading(hdg);

			AddWaypoint(std::make_pair(type, wpt));
		}
		catch (const std::exception& e)
		{
			QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", std::format("Failed to parse waypoint : {} !", name).c_str(), QMessageBox::Ok).exec();
			m_logger->error(std::format("Failed to parse waypoint : {} ! Error : {}.", name, e.what()));
		}
	}
}

void MainWindow::on_export_file_clicked()
{
	std::string fileName = QFileDialog::getSaveFileName(this, "Export Waypoints", "", "XML File (*.xml)").toStdString();
	if (fileName.empty())
		return;

	pugi::xml_document doc;
	pugi::xml_node declaration = doc.append_child(pugi::node_declaration);
	declaration.append_attribute("version") = "1.0";
	declaration.append_attribute("encoding") = "ISO-8859-1";
	declaration.append_attribute("standalone") = "yes";

	pugi::xml_node root = doc.append_child("Aircraft");
	root.append_attribute("name") = "F14";

	// Navgrid
	pugi::xml_node navgrid = root.append_child("Navgrid");
	navgrid.append_attribute("enabled") = m_ui.navgrid_btn->isChecked();
	navgrid.append_attribute("lattitude") = m_ui.navgrid_lat->text().toStdString().c_str();
	navgrid.append_attribute("longitude") = m_ui.navgrid_lon->text().toStdString().c_str();
	navgrid.append_attribute("bearing") = m_ui.navgrid_bearing->value();
	navgrid.append_attribute("width") = m_ui.navgrid_width->value();
	navgrid.append_attribute("sectors") = m_ui.navgrid_sectors->value();

	// Waypoints
	pugi::xml_node wpts = root.append_child("Waypoints");
	for (const auto& wpt : m_waypoints)
	{
		pugi::xml_node wpt_node = wpts.append_child("Waypoint");
		wpt_node.append_attribute("type") = wpt.first.c_str();
		wpt_node.append_attribute("lattitude") = wpt.second.GetLattitude().c_str();
		wpt_node.append_attribute("longitude") = wpt.second.GetLongitude().c_str();
		wpt_node.append_attribute("name") = wpt.second.GetName().c_str();
		wpt_node.append_attribute("altitude") = wpt.second.GetAltitude();
		wpt_node.append_attribute("speed") = wpt.second.GetSpeed();
		wpt_node.append_attribute("heading") = wpt.second.GetHeading();
	}

	if (!doc.save_file(fileName.c_str()))
	{
		QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", "Failed to save xml file !", QMessageBox::Ok).exec();
		m_logger->error(std::format("Failed to save xml file : {} !", fileName));
	}
}

void MainWindow::on_import_cf_clicked()
{
	CombatFlite cf;
	cf.RequestFile();
	cf.ReadFile();
	const auto& waypoints = cf.GetWaypoints();

	for (const auto& wpt : waypoints)
	{
		WaypointAddDialog wad(m_availableWaypoints);
		wad.SetData(wpt);
		wad.SetFunctions([&] {
			const auto& data = wad.GetData();
			if (data.first.empty())
				return;
			AddWaypoint(data);
			wad.close();
			}, [&] {
				wad.close();
			});
		wad.exec();
	}
}

void MainWindow::on_export_ac_clicked()
{
	Navgrid navgrid(m_ui.navgrid_lat->text().toStdString(), m_ui.navgrid_lon->text().toStdString(), m_ui.navgrid_width->value(), m_ui.navgrid_bearing->value(), m_ui.navgrid_sectors->value());
	if (m_ui.navgrid_btn->isChecked())
		m_connector.EnterNavgrid(navgrid);
	m_connector.EnterWaypoints(m_waypoints);
}

const std::pair<const std::string, const Waypoint> MainWindow::FindWaypoint(const std::string& ui_text)
{
	return *std::find_if(m_waypoints.begin(), m_waypoints.end(), [&](const auto& wpt) {
		return wpt.first == ui_text.substr(0, ui_text.find(' '));
		});
}

void MainWindow::AddWaypoint(const std::pair<const std::string, const Waypoint>& data)
{
	m_waypoints.push_back(data);
	m_ui.wpt_list->addItem(std::format("{} - {}", data.first, data.second.GetName()).c_str());
	m_availableWaypoints.removeAll(data.first.c_str());
}

void MainWindow::RemoveWaypoint(const std::pair<const std::string, const Waypoint>& data)
{
	m_waypoints.remove_if([&](const auto& wpt) {
		return wpt.first == data.first;
		});
	qDeleteAll(m_ui.wpt_list->findItems(std::format("{} - {}", data.first, data.second.GetName()).c_str(), Qt::MatchExactly));
	m_availableWaypoints.append(data.first.c_str());
}
