#pragma once

#include <QDialog>

#include "../Logger.h"
#include "ui_FlightSelector.h"

class JTFF_DTC_API_EXPORT FlightSelector : public QDialog
{
	Q_OBJECT

public:
	FlightSelector(QDialog* parent = nullptr);
	~FlightSelector();

	void SetFlights(const std::list<std::string> flights);
	QPushButton* GetButton() { return m_ui.ok_btn; }
	const std::string GetSelected() { return m_ui.selector->currentText().toStdString(); }

private:
	Ui::FlightSelector m_ui;
	std::list<std::string> m_flights;
};
