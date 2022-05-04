#pragma once

#include <QtWidgets/QMainWindow>

#include <list>
#include <utility>

#include <Utilities/Waypoint.h>
#include <Windows/WaypointAddDialog.h>

#include <Logger.h>

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

	void on_import_file_clicked();
	void on_export_file_clicked();
	void on_import_cf_clicked();
	void on_export_ac_clicked();

private:
	static constexpr int m_maxWaypoints = 8;

	Ui::MainWindow m_ui;
	std::list<std::pair<std::string, Waypoint>> m_waypoints;
	WaypointAddDialog m_wpt_dialog;
};
