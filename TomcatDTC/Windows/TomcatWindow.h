#ifndef TOMCATWINDOW_H
#define TOMCATWINDOW_H

#include <QWidget>
#include <QMessageBox>

#include <string>

#include <JTFF_DTC_API.h>
#include "ui_TomcatWindow.h"
#include "../Connectors/TomcatConnector.h"

class TomcatWindow : public QWidget
{
	Q_OBJECT

public:
	TomcatWindow(QWidget* parent, const ConnectorCreator& connector, DTCLogger* log, DTCSettings* settings);
	~TomcatWindow();

public slots:
	void OpenWaypointDialog();
	void AddWaypoint();
	void RemoveWaypoint();
	void ModifyWaypoint();
	void ExportToAircraft();
	void ExportToFile();
	void ImportFromFile();
	void OpenSelectFileDiaglogCF();
	void ImportFromCombatFilte();
	void CancelAdd();

private:
	void AddCFWaypoints();

	std::map<std::string, std::map<std::string, std::string>> m_flightData;

	Ui::TomcatWindow m_ui;

	std::unordered_map<Tomcat::UsableWaypoints, std::string> m_waypoints;
	std::vector<QListWidgetItem*> m_waypointListItems;

	TomcatConnector m_connector;
	WaypointAddDialog* m_dialog;
	FileSelectorWindow* m_selectWindow;

	DTCLogger* m_logger;
	DTCSettings* m_settings;
};

#endif //!TOMCATWINDOW_H