#include "Navgrid.h"

Navgrid::Navgrid(const std::string& lat, const std::string& lon, const int& width, const int& heading, const int& sectors)
	: m_lat(Coordinate::FromString(lat)), m_lon(Coordinate::FromString(lon)), m_width(width), m_bearing(heading), m_sectors(sectors)
{
}
