#pragma once

#include <Utilities/Coordinate.h>

class Navgrid
{
public:
	Navgrid(const std::string& lat, const std::string& lon, const int& width, const int& bearing, const int& sectors);
	~Navgrid() = default;

	const std::string& GetLattitude() const { return m_lat.ToString(); }
	const std::string& GetLongitude() const { return m_lon.ToString(); }
	const int& GetWidth() const { return m_width; }
	const int& GetBearing() const { return m_bearing; }
	const int& GetSectors() const { return m_sectors; }

private:
	const Coordinate m_lat, m_lon;
	const int m_width, m_bearing, m_sectors;
};