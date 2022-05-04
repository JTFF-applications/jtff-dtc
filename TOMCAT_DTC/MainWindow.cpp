#include <QMessageBox>

#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);

	m_availableWaypoints = QList<QString>::fromVector({ "WP1", "WP2", "WP3", "FP", "IP", "HB", "DP", "HA" });

	QObject::connect(m_ui.add_wpt_btn, &QPushButton::clicked, this, &MainWindow::on_add_wpt_clicked);
	QObject::connect(m_ui.rm_wpt_btn, &QPushButton::clicked, this, &MainWindow::on_rm_wpt_clicked);
	QObject::connect(m_ui.modify_wpy_btn, &QPushButton::clicked, this, &MainWindow::on_modify_wpt_clicked);
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
				m_availableWaypoints.removeOne(wpt.first.c_str());
				wad.close();
			});
		wad.exec();
	}
}

void MainWindow::on_import_file_clicked()
{
}

void MainWindow::on_export_file_clicked()
{
}

void MainWindow::on_import_cf_clicked()
{
}

void MainWindow::on_export_ac_clicked()
{
}

const std::pair<const std::string, const Waypoint> MainWindow::FindWaypoint(const std::string& ui_text)
{
	return *std::find_if(m_waypoints.begin(), m_waypoints.end(), [&](const auto& wpt) {
		return wpt.first == ui_text.substr(0, ui_text.find(' '));
		});
}

void MainWindow::AddWaypoint(const std::pair <const std::string, const Waypoint>& data)
{
	m_waypoints.push_back(data);
	m_ui.wpt_list->addItem(std::format("{} - {}", data.first, data.second.GetName()).c_str());
	m_availableWaypoints.removeOne(data.first.c_str());
}

void MainWindow::RemoveWaypoint(const std::pair<const std::string, const Waypoint>& data)
{
	m_waypoints.remove_if([&](const auto& wpt) {
		return wpt.first == data.first;
		});
	qDeleteAll(m_ui.wpt_list->findItems(std::format("{} - {}", data.first, data.second.GetName()).c_str(), Qt::MatchExactly));
	m_availableWaypoints.append(data.first.c_str());
}
