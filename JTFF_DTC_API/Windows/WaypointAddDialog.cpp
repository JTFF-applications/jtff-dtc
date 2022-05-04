#include <QMessageBox>

#include "WaypointAddDialog.h"

WaypointAddDialog::WaypointAddDialog(QWidget* parent)
	: QWidget(parent), m_logger(Logger::Get())
{
	m_ui.setupUi(this);

	m_ui.speed->setEnabled(true);
	m_ui.heading->setEnabled(true);

	m_ui.waypoints->addItems({ "WP1", "WP2", "WP3", "FP", "IP", "ST", "HA", "DP", "HB" });
	m_ui.altitude->setValidator(new QIntValidator(this));
	m_ui.speed->setValidator(new QIntValidator(0, 10000, this));
	m_ui.heading->setValidator(new QIntValidator(0, 360, this));

	m_onAccept = [] {};
	m_onReject = [] {};

	QObject::connect(m_ui.ok_btn, &QPushButton::clicked, m_onAccept);
	QObject::connect(m_ui.cancel_btn, &QPushButton::clicked, m_onReject);
}

WaypointAddDialog::~WaypointAddDialog()
{
	delete m_ui.altitude->validator(), m_ui.speed->validator(), m_ui.heading->validator();
}

void WaypointAddDialog::SetFucntions(std::function<void(void)> onAccept, std::function<void(void)> onReject)
{
	m_onAccept = onAccept;
	m_onReject = onReject;
}

std::pair<std::string, Waypoint> WaypointAddDialog::GetData()
{
	try
	{
		std::string pos = m_ui.position->text().toStdString();

		Waypoint wpt = Waypoint::FromStringCoordinates(pos.substr(0, pos.find(' ')), pos.substr(pos.find(' ') + 1, pos.size() - 1));
		wpt.SetName(m_ui.name->text().toStdString());
		wpt.SetAltitude(m_ui.altitude->text().toInt());
		wpt.SetSpeed(m_ui.speed->text().toInt());
		wpt.SetHeading(m_ui.heading->text().toInt());

		return std::make_pair(m_ui.waypoints->currentData().toString().toStdString(), wpt);
	}
	catch (const Coordinate::bad_coordinate& except)
	{
		QMessageBox(QMessageBox::Icon::Critical, "Waypoint Error", except.what(), QMessageBox::StandardButton::Ok).exec();
	}
	return std::make_pair("", Waypoint::FromDecimalCoordinates("0.0", "0.0"));
}

void WaypointAddDialog::SetData(const Waypoint& wpt)
{
	m_ui.position->setText(wpt.GetCoordinates().c_str());
	m_ui.name->setText(wpt.GetName().c_str());
	m_ui.altitude->setText(QString::number(wpt.GetAltitude()));
	m_ui.speed->setText(QString::number(wpt.GetSpeed()));
	m_ui.heading->setText(QString::number(wpt.GetHeading()));
}

void WaypointAddDialog::hide()
{
	m_onAccept = [] {};
	m_onReject = [] {};
	QWidget::hide();
}
