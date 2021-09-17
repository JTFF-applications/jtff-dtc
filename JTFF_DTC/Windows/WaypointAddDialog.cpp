#include "WaypointAddDialog.h"

WaypointAddDialog::WaypointAddDialog(QWidget* parent, const std::string& aircraft, DTCLogger* log, DTCSettings* settings)
	: QWidget(parent), m_logger(log), m_settings(settings)
{
	m_ui.setupUi(this);

	if (aircraft == "TOMCAT")
		m_ui.WP->addItems({ "WP1", "WP2", "WP3", "FP", "IP", "ST", "HA", "DP", "HB" });
	else if (aircraft == "HORNET")
		for (int i = 0; i < HornetConnector::GetMaxWaypoints(); i++)
			m_ui.WP->addItem(std::to_string(i).c_str());
}

WaypointAddDialog::~WaypointAddDialog()
{
}
