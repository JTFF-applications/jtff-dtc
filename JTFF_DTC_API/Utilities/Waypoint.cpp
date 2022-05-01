#include "Waypoint.h"

Waypoint Waypoint::FromStringCoordinates(const std::string& lat_dms, const std::string& lon_dms, const std::string& name, const int& alt, const int& speed, const int& heading)
{
	Waypoint wpt;
	wpt.m_lat = Coordinate::FromString(lat_dms);
	wpt.m_lon = Coordinate::FromString(lon_dms);
	wpt.m_name = name;
	wpt.m_altitude = alt;
	wpt.m_speed = speed;
	wpt.m_heading = heading;
	return wpt;
}

Waypoint Waypoint::FromDecimalCoordinates(const std::string& lat_decimal, const std::string& lon_decimal, const std::string& name, const int& alt, const int& speed, const int& heading)
{
	Waypoint wpt;
	wpt.m_lat = Coordinate::FromDecimal(lat_decimal, Coordinate::Type::Lattitude);
	wpt.m_lon = Coordinate::FromDecimal(lon_decimal, Coordinate::Type::Longitude);
	wpt.m_name = name;
	wpt.m_altitude = alt;
	wpt.m_speed = speed;
	wpt.m_heading = heading;
	return wpt;
}

Waypoint Waypoint::FromStringCoordinates(const std::string& lat_dms, const std::string& lon_dms)
{
	Waypoint wpt;
	wpt.m_lat = Coordinate::FromString(lat_dms);
	wpt.m_lon = Coordinate::FromString(lon_dms);
	return wpt;
}

Waypoint Waypoint::FromDecimalCoordinates(const std::string& lat_decimal, const std::string& lon_decimal)
{
	Waypoint wpt;
	wpt.m_lat = Coordinate::FromDecimal(lat_decimal, Coordinate::Type::Lattitude);
	wpt.m_lon = Coordinate::FromDecimal(lon_decimal, Coordinate::Type::Longitude);
	return wpt;
}
