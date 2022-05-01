#pragma once

#include <string>

#include "Coordinate.h"

class Waypoint
{
public:
	~Waypoint() = default;

	const std::string GetCoordinates() const { return std::format("{} {}", m_lat.ToString(), m_lon.ToString()); }
	const std::string& GetName() const { return m_name; }
	const int& GetAltitude() const { return m_altitude; }
	const int& GetSpeed() const { return m_speed; }
	const int& GetHeading() const { return m_heading; }

	void SetLattitude(const Coordinate& lat) { m_lat = lat; }
	void SetLongitude(const Coordinate& lon) { m_lon = lon; }
	void SetName(const std::string& name) { m_name = name; }
	void SetAltitude(const int& altitude) { m_altitude = altitude; }
	void SetSpeed(const int& speed) { m_speed = speed; }
	void SetHeading(const int& heading) { m_heading = heading; }

public:
	static Waypoint FromStringCoordinates(const std::string& lat_dms, const std::string& lon_dms, const std::string& name, const int& alt, const int& speed, const int& heading);
	static Waypoint FromDecimalCoordinates(const std::string& lat_decimal, const std::string& lon_decimal, const std::string& name, const int& alt, const int& speed, const int& heading);
	static Waypoint FromStringCoordinates(const std::string& lat_dms, const std::string& lon_dms);
	static Waypoint FromDecimalCoordinates(const std::string& lat_decimal, const std::string& lon_decimal);

private:
	Waypoint() = default;

private:
	Coordinate m_lat, m_lon;
	int m_altitude = 0, m_speed = 0, m_heading = 0;
	std::string m_name = "";
};
