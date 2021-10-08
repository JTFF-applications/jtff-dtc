#include "WaypointAddDialog.h"

WaypointAddDialog::WaypointAddDialog(QWidget* parent, const DTCUtils::Aircraft& aircraft, DTCLogger* log, DTCSettings* settings)
	: QWidget(parent), m_logger(log), m_settings(settings)
{
	m_ui.setupUi(this);

	if (aircraft == DTCUtils::Aircraft::Tomcat)
		m_ui.WP->addItems({ "WP1", "WP2", "WP3", "FP", "IP", "ST", "HA", "DP", "HB" });
	else if (aircraft == DTCUtils::Aircraft::Hornet)
		for (int i = 0; i < Hornet::MaxWaypoints; i++)
			m_ui.WP->addItem(std::to_string(i).c_str());
}

WaypointAddDialog::~WaypointAddDialog()
{
}
