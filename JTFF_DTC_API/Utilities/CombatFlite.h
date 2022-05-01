#pragma once

#include <list>

#include "../Windows/FlightSelector.h"

#include "Waypoint.h"
#include "../Logger.h"

class JTFF_DTC_API_EXPORT CombatFlite
{
public:
	CombatFlite();
	~CombatFlite() = default;

	void RequestFile();
	void ReadFile();
	const std::list<Waypoint> GetWaypoints() const { return m_waypoints; }

private:
	void RequestFlight();
	const std::list<std::string> GetFlightsFromFile();
	std::list<Waypoint> GetWaypointsFromFile();

private:
	std::string m_path, m_flight;
	std::list<Waypoint> m_waypoints;
	std::shared_ptr<spdlog::logger> m_logger;
	FlightSelector m_selector;
};
