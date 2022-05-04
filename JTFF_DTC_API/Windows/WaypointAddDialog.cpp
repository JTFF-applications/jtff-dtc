#include <QMessageBox>

#include "WaypointAddDialog.h"

WaypointAddDialog::WaypointAddDialog(const QStringList& available_wpts, QWidget* parent)
	: QDialog(parent), m_logger(Logger::Get())
{
	m_ui.setupUi(this);

	m_ui.speed->setEnabled(true);
	m_ui.heading->setEnabled(true);

	m_ui.waypoints->addItems(available_wpts);
	m_ui.altitude->setValidator(new QIntValidator(this));
	m_ui.speed->setValidator(new QIntValidator(0, 10000, this));
	m_ui.heading->setValidator(new QIntValidator(0, 360, this));
}

WaypointAddDialog::~WaypointAddDialog()
{
	delete m_ui.altitude->validator(), m_ui.speed->validator(), m_ui.heading->validator();
}

void WaypointAddDialog::SetFunctions(std::function<void(void)> onAccept, std::function<void(void)> onReject)
{
	m_onAccept = onAccept;
	m_onReject = onReject;

	QObject::connect(m_ui.ok_btn, &QPushButton::clicked, m_onAccept);
	QObject::connect(m_ui.cancel_btn, &QPushButton::clicked, m_onReject);
}

void WaypointAddDialog::SetCurrentWpt(const std::string& wpt)
{
	m_ui.waypoints->setCurrentText(wpt.c_str());
}

void WaypointAddDialog::SetData(const Waypoint& wpt)
{
	m_ui.position->setText(wpt.GetCoordinates().c_str());
	m_ui.name->setText(wpt.GetName().c_str());
	m_ui.altitude->setText(QString::number(wpt.GetAltitude()));
	m_ui.speed->setText(QString::number(wpt.GetSpeed()));
	m_ui.heading->setText(QString::number(wpt.GetHeading()));
}

std::pair<const std::string, const Waypoint> WaypointAddDialog::GetData()
{
	try
	{
		std::string pos = m_ui.position->text().toStdString();
		if (pos.empty())
			throw Coordinate::bad_coordinate("Empty coordinate !");

		Waypoint wpt = Waypoint::FromStringCoordinates(pos.substr(0, pos.find(' ')), pos.substr(pos.find(' ') + 1, pos.size() - 1));
		wpt.SetName(m_ui.name->text().toStdString());
		wpt.SetAltitude(m_ui.altitude->text().toInt());
		wpt.SetSpeed(m_ui.speed->text().toInt());
		wpt.SetHeading(m_ui.heading->text().toInt());

		return std::make_pair(m_ui.waypoints->currentText().toStdString(), wpt);
	}
	catch (const Coordinate::bad_coordinate& except)
	{
		QMessageBox(QMessageBox::Icon::Critical, "Waypoint Error", except.what(), QMessageBox::StandardButton::Ok).exec();
	}
	return std::make_pair("", Waypoint::FromDecimalCoordinates("0.0", "0.0"));
}
