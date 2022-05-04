#pragma once

#include <QtWidgets/QMainWindow>

#include <list>
#include <utility>

#include <Utilities/Waypoint.h>
#include <Logger.h>

#include "TomcatConnector.h"
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

private slots:
	void on_add_wpt_clicked();
	void on_rm_wpt_clicked();
	void on_modify_wpt_clicked();

	void on_navgrid_validate_clicked();
	void on_navgrid_coordinates_changed();

	void on_import_file_clicked();
	void on_export_file_clicked();
	void on_import_cf_clicked();
	void on_export_ac_clicked();

private:
	const std::pair<const std::string, const Waypoint> FindWaypoint(const std::string& ui_text);
	void AddWaypoint(const std::pair<const std::string, const Waypoint>& data);
	void RemoveWaypoint(const std::pair<const std::string, const Waypoint>& data);

private:
	Ui::MainWindow m_ui;
	TomcatConnector m_connector;

	static constexpr int m_maxWaypoints = 8;
	std::shared_ptr<spdlog::logger> m_logger;

	std::list<std::pair<const std::string, const Waypoint>> m_waypoints;
	QList<QString> m_availableWaypoints;
};
