#ifndef WAYPOINTADDDIALOG_H
#define WAYPOINTADDDIALOG_H

#include <QWidget>

#include "../DTCUtils.h"
#include "../Connectors/TomcatConnector.h"
#include "ui_WaypointAddDialog.h"

class WaypointAddDialog : public QWidget
{
	Q_OBJECT

public:
	WaypointAddDialog(QWidget* parent, const std::string& aircraft, DTCLogger* log, DTCSettings* settings);
	~WaypointAddDialog();

	Ui::WaypointAddDialog m_ui;

private:
	DTCLogger* m_logger;
	DTCSettings* m_settings;
};

#endif //!WAYPOINTADDDIALOG_H
