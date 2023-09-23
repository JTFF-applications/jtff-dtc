#include <QFileDialog>
#include <QMessageBox>

#include <fstream>

#include <pugixml.hpp>

#include <Windows/WaypointAddDialog.h>
#include <Utilities/CombatFlite.h>

#include "Navgrid.h"
#include "MainWindow.h"
#include "TomcatXmlParser.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), m_logger(Logger::Get()), m_connector("127.0.0.1", 7778)
{
	m_ui.setupUi(this);

	m_availableWaypoints = QList<QString>::fromVector({ "WP1", "WP2", "WP3", "FP", "IP", "HB", "DP", "HA", "ST" });

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

	m_logger->info("App Started !");
}

MainWindow::~MainWindow()
{
	m_logger->info("App closed !");
}

void MainWindow::on_add_wpt_clicked()
{
	if (m_waypoints.size() + 1 > m_maxWaypoints)
	{
		QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", "You can't enter more waypoint for the tomcat !", QMessageBox::Ok).exec();
		m_logger->info("You can't enter more waypoint for the tomcat !");
		return;
	}

	WaypointAddDialog wad(m_availableWaypoints);
	wad.SetFunctions([&] {
		auto data = wad.GetData();
		if (data.first.empty())
			return;
		AddWaypoint(data);
		wad.close();
		m_logger->info("Waypoint {} added", data.first);
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
        std::string wpt = item->text().toStdString();
        RemoveWaypoint(FindWaypoint(wpt));
        m_logger->info("Waypoint {} removed", wpt);
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
			m_logger->info("Waypoint {} modified", data.first);
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
	const std::string fileName = QFileDialog::getOpenFileName(this, "Open XML file", "", "XML files (*.xml)").toStdString();
	if (fileName.empty())
		return;

	try
	{
		TomcatXmlParser parser("F14", fileName);
		Navgrid navgrid = parser.GetNavgrid();
		m_ui.navgrid_btn->setChecked(navgrid.GetChecked());
		m_ui.navgrid_lat->setText(navgrid.GetLattitude().c_str());
		m_ui.navgrid_lon->setText(navgrid.GetLongitude().c_str());
		m_ui.navgrid_bearing->setValue(navgrid.GetBearing());
		m_ui.navgrid_width->setValue(navgrid.GetWidth());
		m_ui.navgrid_sectors->setValue(navgrid.GetSectors());
		m_logger->info("Navgrid imported");

		const std::list<std::pair<const std::string, const Waypoint>> waypoints = parser.GetWaypoints();
		for (const auto& wpt : waypoints)
		{
			if (m_waypoints.size() + 1 > m_maxWaypoints)
			{
				QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", "You can't enter more waypoint for the tomcat !", QMessageBox::Ok).exec();
				m_logger->info("You can't enter more waypoint for the tomcat !");
				return;
			}

			AddWaypoint(wpt);
			m_logger->info("Waypoint {} imported", wpt.first);
		}
	}
	catch (const BaseXmlParser::bad_parser& except)
	{
		switch (except.error())
		{
		case BaseXmlParser::ERROR_TYPE::BAD_FILE:
			QMessageBox(QMessageBox::Icon::Warning, "XML Error", "No file found !", QMessageBox::Ok).exec();
			m_logger->critical("File {} not found or not readable", fileName);
			break;
		case BaseXmlParser::ERROR_TYPE::BAD_AIRCRAFT:
			QMessageBox(QMessageBox::Icon::Warning, "XML Error", "Bad aircraft (not F14) !", QMessageBox::Ok).exec();
			m_logger->critical("Bad aircraft in file {}", fileName);
			break;
		}
	}
}

void MainWindow::on_export_file_clicked()
{
	TomcatXmlParser parser("F14");

	const bool isChecked = m_ui.navgrid_btn->isChecked();
	const std::string navgrid_lattitude = m_ui.navgrid_lat->text().toStdString();
	const std::string navgrid_longitude = m_ui.navgrid_lon->text().toStdString();
	const int navgrid_bearing = m_ui.navgrid_bearing->value();
	const int navgrid_width = m_ui.navgrid_width->value();
	const int navgrid_sectors = m_ui.navgrid_sectors->value();

	Navgrid navgrid(isChecked, navgrid_lattitude, navgrid_longitude, navgrid_width, navgrid_bearing, navgrid_sectors);
	try
	{
		parser.WriteNavgrid(navgrid);
		parser.WriteWaypoints(m_waypoints);
	}
	catch (const BaseXmlParser::bad_parser& except)
	{
		m_logger->error("Error while writing the XML file : {}", except.what());
		QMessageBox(QMessageBox::Icon::Critical, "XML Error", "Error while writing the XML file !", QMessageBox::Ok).exec();
		return;
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
		if (m_waypoints.size() + 1 > m_maxWaypoints)
		{
			QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", "You can't enter more waypoint for the tomcat !", QMessageBox::Ok).exec();
			m_logger->info("You can't enter more waypoint for the tomcat !");
			return;
		}

		WaypointAddDialog wad(m_availableWaypoints);
		wad.SetData(wpt);
		wad.SetFunctions([&] {
			const auto& data = wad.GetData();
			if (data.first.empty())
				return;
			AddWaypoint(data);
			m_logger->info("Waypoint {} imported from CF", data.first);
			wad.close();
			}, [&] {
				wad.close();
			});
		wad.exec();
	}
}

void MainWindow::on_export_ac_clicked()
{
	if (m_ui.navgrid_btn->isChecked())
	{
		try
		{
			Navgrid navgrid(true, m_ui.navgrid_lat->text().toStdString(), m_ui.navgrid_lon->text().toStdString(), m_ui.navgrid_width->value(), m_ui.navgrid_bearing->value(), m_ui.navgrid_sectors->value());
			m_connector.EnterNavgrid(navgrid);
		}
		catch (const std::exception&)
		{
			QMessageBox(QMessageBox::Icon::Critical, "Tomcat Error", "Failed to enter navgrid.", QMessageBox::Ok).exec();
			return;
		}
	}
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
