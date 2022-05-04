#include "FlightSelector.h"

FlightSelector::FlightSelector(QDialog* parent)
	: QDialog(parent)
{
	m_ui.setupUi(this);
}

FlightSelector::~FlightSelector()
{
}

void FlightSelector::SetFlights(const std::list<std::string> flights)
{
	m_ui.selector->clear();
	for (const auto flight : flights)
		m_ui.selector->addItem(flight.c_str());
}
