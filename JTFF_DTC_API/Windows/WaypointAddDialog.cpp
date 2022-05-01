#include "WaypointAddDialog.h"

WaypointAddDialog::WaypointAddDialog(std::function<void(void)> onAccept, std::function<void(void)> onReject, QWidget* parent)
	: QWidget(parent), m_logger(Logger::Get()), m_onAccept(onAccept), m_onReject(onReject)
{
	m_ui.setupUi(this);

	m_ui.speed->setEnabled(true);
	m_ui.heading->setEnabled(true);

	m_ui.waypoints->addItems({ "WP1", "WP2", "WP3", "FP", "IP", "ST", "HA", "DP", "HB" });
	m_ui.altitude->setValidator(new QIntValidator(this));
	m_ui.speed->setValidator(new QIntValidator(0, 10000, this));
	m_ui.heading->setValidator(new QIntValidator(0, 360, this));

	QObject::connect(m_ui.ok_btn, &QPushButton::clicked, m_onAccept);
	QObject::connect(m_ui.cancel_btn, &QPushButton::clicked, m_onReject);
}

WaypointAddDialog::~WaypointAddDialog()
{
	delete m_ui.altitude->validator(), m_ui.speed->validator(), m_ui.heading->validator();
}

void WaypointAddDialog::Set(const Waypoint& wpt)
{
	m_ui.position->setText(wpt.GetCoordinates().c_str());
	m_ui.name->setText(wpt.GetName().c_str());
	m_ui.altitude->setText(QString::number(wpt.GetAltitude()));
	m_ui.speed->setText(QString::number(wpt.GetSpeed()));
	m_ui.heading->setText(QString::number(wpt.GetHeading()));
}
