#include "Navgrid.h"

Navgrid::Navgrid(const bool& isChecked, const std::string& lat, const std::string& lon, const int& width, const int& heading, const int& sectors)
	: m_checked(isChecked), m_lat(Coordinate::FromString(isChecked ? lat : "N00.00.000")), m_lon(Coordinate::FromString(isChecked ? lon : "E000.00.000")), m_width(width), m_bearing(heading), m_sectors(sectors)
{
}
