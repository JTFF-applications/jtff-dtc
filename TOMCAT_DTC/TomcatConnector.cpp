#include "TomcatConnector.h"

const int WpToCap(const std::string& wp)
{
	if (wp == "WP1")
		return 1;
	else if (wp == "WP2")
		return 2;
	else if (wp == "WP3")
		return 3;
	else if (wp == "FP")
		return 4;
	else if (wp == "IP")
		return 5;
	else if (wp == "HB")
		return 6;
	else if (wp == "DP")
		return 7;
	else if (wp == "HA")
		return 8;
	else if (wp == "ST")
		return 9;
	throw std::out_of_range(std::format("Unknown waypoint {} !", wp));
}

TomcatConnector::TomcatConnector(const std::string& host, const int& port)
	: BaseConnector(host, port)
{
}

void TomcatConnector::EnterWaypoints(std::list<std::pair<const std::string, const Waypoint>> waypoints)
{
	for (const auto& wpt : waypoints)
		EnterWaypoint(wpt);
}

void TomcatConnector::EnterWaypoint(std::pair<const std::string, const Waypoint> waypoint)
{
	SetCAP("TAC DATA");
	SetCAP(std::format("CAP_{}", WpToCap(waypoint.first)));

	// Lattitude
	SetCAP("CLEAR");
	SetCAP("1");
	SetCAP(std::string(1, waypoint.second.GetLattitude().front()));
	EnterCoordinates(waypoint.second.GetLattitude().substr(1));
	SetCAP("ENTER");

	// Longitude
	SetCAP("CLEAR");
	SetCAP("6");
	SetCAP(std::string(1, waypoint.second.GetLongitude().front()));
	EnterCoordinates(waypoint.second.GetLongitude().substr(1));
	SetCAP("ENTER");

	// Altitude
	if (waypoint.second.GetAltitude() != 0)
	{
		SetCAP("CLEAR");
		SetCAP("4");
		SetCAP("N");
		EnterNumber(waypoint.second.GetAltitude());
		SetCAP("ENTER");
	}

	// Speed
	if (waypoint.second.GetSpeed() != 0)
	{
		SetCAP("CLEAR");
		SetCAP("3");
		EnterNumber(waypoint.second.GetSpeed());
		SetCAP("ENTER");
	}

	// Heading
	if (waypoint.second.GetHeading() != 0)
	{
		SetCAP("CLEAR");
		SetCAP("8");
		EnterNumber(waypoint.second.GetHeading());
		SetCAP("ENTER");
	}
}

void TomcatConnector::EnterNavgrid(const Navgrid& navgrid)
{
	SetTID("GND STAB");
	SetCAP("D/L");
	SetCAP("CAP_3");

	// Lattitude
	SetCAP("CLEAR");
	SetCAP("1");
	SetCAP(std::string(1, navgrid.GetLattitude().front()));
	EnterCoordinates(navgrid.GetLattitude());
	SetCAP("ENTER");

	// Longitude
	SetCAP("CLEAR");
	SetCAP("6");
	SetCAP(std::string(1, navgrid.GetLongitude().front()));
	EnterCoordinates(navgrid.GetLongitude());
	SetCAP("ENTER");

	// Settors
	SetCAP("CLEAR");
	SetCAP("2");
	EnterNumber(navgrid.GetSectors());
	SetCAP("ENTER");

	// Width
	SetCAP("CLEAR");
	SetCAP("4");
	EnterNumber(navgrid.GetWidth());
	SetCAP("ENTER");

	// Bearing
	SetCAP("CLEAR");
	SetCAP("8");
	EnterNumber(navgrid.GetBearing());
	SetCAP("ENTER");
}

void TomcatConnector::SetCAP(const std::string& num)
{
	if (num.empty())
		return;

	if (num == "BIT")
		SendKey("RIO_CAP_CATRGORY 0", true, 50, 1000);
	else if (num == "SPL")
		SendKey("RIO_CAP_CATRGORY 1", true, 50, 1000);
	else if (num == "NAV")
		SendKey("RIO_CAP_CATRGORY 2", true, 50, 1000);
	else if (num == "TAC DATA")
		SendKey("RIO_CAP_CATRGORY 3", true, 50, 1000);
	else if (num == "D/L")
		SendKey("RIO_CAP_CATRGORY 4", true, 50, 1000);
	else if (num == "TGT DATA")
		SendKey("RIO_CAP_CATRGORY 5", true, 50, 1000);

	if (m_capKeys.count(num))
		SendKey(m_capKeys.at(num));
}

void TomcatConnector::SetTID(const std::string& num)
{
	if (num.empty())
		return;

	if (num == "GND STAB")
		SendKey("RIO_TID_MODE 0", true, 500, 500);
	else if (num == "A/C STAB")
		SendKey("RIO_TID_MODE 1", true, 500, 500);
	else if (num == "ATTK")
		SendKey("RIO_TID_MODE 2", true, 500, 500);
	else if (num == "TV")
		SendKey("RIO_TID_MODE 3", true, 500, 500);
}

void TomcatConnector::EnterCoordinates(const std::string& coordinates)
{
	for (const char& digit : coordinates)
		if (std::isdigit(digit))
			SetCAP(std::string(1, digit));
}

void TomcatConnector::EnterNumber(const int& nb)
{
	const std::string number = std::to_string(nb);
	for (const char& digit : number)
		SetCAP(std::string(1, digit));
}
