#ifndef WAYPOINTADDDIALOG_H
#define WAYPOINTADDDIALOG_H

#include <QWidget>

#include "ui_WaypointAddDialog.h"
#include "../DTCUtils.h"
#include "../AircraftSpecials.h"

class JTFF_DTC_API WaypointAddDialog : public QWidget
{
	Q_OBJECT

public:
	WaypointAddDialog(QWidget* parent, const DTCUtils::Aircraft& aircraft, DTCLogger* log, DTCSettings* settings);
	~WaypointAddDialog();

	Ui::WaypointAddDialog m_ui;

private:
	DTCLogger* m_logger;
	DTCSettings* m_settings;
};

#endif //!WAYPOINTADDDIALOG_H
