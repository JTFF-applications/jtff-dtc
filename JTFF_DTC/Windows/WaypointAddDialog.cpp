#include "WaypointAddDialog.h"

WaypointAddDialog::WaypointAddDialog(QWidget *parent, const std::string& aircraft, DTCLogger* log, DTCSettings* settings)
	: QWidget(parent), m_logger(log), m_settings(settings)
{
	m_ui.setupUi(this);

	if (aircraft == "TOMCAT")
		m_ui.WP->addItems({ "WP1", "WP2", "WP3", "FP", "IP", "ST", "HA", "DP", "HB" });
}

WaypointAddDialog::~WaypointAddDialog()
{
}
