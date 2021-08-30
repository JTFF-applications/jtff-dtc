#ifndef HORNETWINDOW_H
#define HORNETWINDOW_H

#include <QWidget>
#include <QListWidget>

#include "../DTCUtils.h"
//#include "../Connectors/HornetConnector.h"
#include "WaypointAddDialog.h"
#include "FileSelectorWindow.h"
#include "ui_HornetWindow.h"

class HornetWindow : public QWidget
{
	Q_OBJECT

public:
	HornetWindow(QWidget* parent, const ConnectorCreator& connector, DTCLogger* log, DTCSettings* settings);
	~HornetWindow();

/*/public slots:
	void OpenWaypointDialog();
	void AddWaypoint();
	void RemoveWaypoint();
	void ModifyWaypoint();
	void ExportToAircraft();
	void ExportToFile();
	void ImportFromFile();
	void OpenSelectFileDiaglogCF();
	void ImportFromCombatFilte();
	void CancelAdd();*/

private:
	void AddCFWaypoints();

	std::map<std::string, std::map<std::string, std::string>> m_flightData;

	Ui::HornetWindow m_ui;

	std::unordered_map<unsigned int, std::string> m_waypoints;
	std::vector<QListWidgetItem*> m_waypointListItems;

	//HornetConnector m_connector;
	WaypointAddDialog* m_dialog;
	FileSelectorWindow* m_selectWindow;

	DTCLogger* m_logger;
	DTCSettings* m_settings;
};

#endif