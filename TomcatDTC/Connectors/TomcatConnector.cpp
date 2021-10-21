#include "TomcatConnector.h"

TomcatConnector::TomcatConnector(const ConnectorCreator& base)
	: BaseConnector(base)
{
}

TomcatConnector::~TomcatConnector()
{
}

void TomcatConnector::enterWaypoints(const std::unordered_map<Tomcat::UsableWaypoints, std::string>& waypoints)
{
	for (auto elem : waypoints)
		enterWaypoint(elem.second, elem.first);
}

void TomcatConnector::setComputerAddressPanel(const std::string& num)
{
	if (num.empty())
		return;

	if (num == "BIT")
		sendKey("RIO_CAP_CATRGORY 0", true, 1000);
	else if (num == "SPL")
		sendKey("RIO_CAP_CATRGORY 1", true, 1000);
	else if (num == "NAV")
		sendKey("RIO_CAP_CATRGORY 2", true, 1000);
	else if (num == "TAC DATA")
		sendKey("RIO_CAP_CATRGORY 3", true, 1000);
	else if (num == "D/L")
		sendKey("RIO_CAP_CATRGORY 4", true, 1000);
	else if (num == "TGT DATA")
		sendKey("RIO_CAP_CATRGORY 5", true, 1000);

	if (m_capKeys.count(num))
		sendKey(m_capKeys[num], false);
}

void TomcatConnector::setTargetInformationDisplay(const std::string& num) {
	if (num.empty())
		return;

	if (num == "GND STAB")
		sendKey("RIO_TID_MODE 0", true, 1000);
	else if (num == "A/C STAB")
		sendKey("RIO_TID_MODE 1", true, 1000);
	else if (num == "ATTK")
		sendKey("RIO_TID_MODE 2", true, 1000);
	else if (num == "TV")
		sendKey("RIO_TID_MODE 3", true, 1000);
}


void TomcatConnector::enterNumber(const std::string& nb)
{
	for (char digit : nb)
		setComputerAddressPanel(std::string(1, digit));
}

void TomcatConnector::enterWaypoint(const std::string& coords, const Tomcat::UsableWaypoints& waypoint)
{
	getLogger()->info("Entering WP" + (int)waypoint);

	Waypoint point = parseCoords(coords);
	setComputerAddressPanel("TAC DATA");

	// Select waypoint
	setComputerAddressPanel("CAP_" + std::to_string((int)waypoint));

	// Set lattitude
	setComputerAddressPanel("CLEAR");
	setComputerAddressPanel("1");
	if (point.N != "") {
		setComputerAddressPanel("N");
		enterNumber(point.N);
	}
	else {
		setComputerAddressPanel("S");
		enterNumber(point.S);
	}
	setComputerAddressPanel("ENTER");

	// Set longitude
	setComputerAddressPanel("CLEAR");
	setComputerAddressPanel("6");
	if (point.E != "") {
		setComputerAddressPanel("E");
		enterNumber(point.E);
	}
	else {
		setComputerAddressPanel("W");
		enterNumber(point.W);
	}
	setComputerAddressPanel("ENTER");

	// Set altitude
	if (point.ALT != "") {
		setComputerAddressPanel("CLEAR");
		setComputerAddressPanel("4");
		setComputerAddressPanel("N");
		enterNumber(point.ALT);
		setComputerAddressPanel("ENTER");
	}
}

void TomcatConnector::enterNavgrid(const Tomcat::NavgridParams& navgrid)
{
	getLogger()->info("Entering navgrid...");

	Waypoint point = parseCoords(navgrid.center, true);
	setTargetInformationDisplay("GND STAB");
	setComputerAddressPanel("D/L");
	setComputerAddressPanel("CAP_3");

	// Enter center point
		// Lattitude
	setComputerAddressPanel("CLEAR");
	setComputerAddressPanel("1");
	if (point.N != "") {
		setComputerAddressPanel("N");
		enterNumber(point.N);
	}
	else {
		setComputerAddressPanel("S");
		enterNumber(point.S);
	}
	setComputerAddressPanel("ENTER");
		// Longitude
	setComputerAddressPanel("CLEAR");
	setComputerAddressPanel("6");
	if (point.E != "") {
		setComputerAddressPanel("E");
		enterNumber(point.E);
	}
	else {
		setComputerAddressPanel("W");
		enterNumber(point.W);
	}
	setComputerAddressPanel("ENTER");

	// Set sectors nb
	setComputerAddressPanel("CLEAR");
	setComputerAddressPanel("2");
	enterNumber(std::to_string(navgrid.sectors));
	setComputerAddressPanel("ENTER");

	// Set width
	setComputerAddressPanel("CLEAR");
	setComputerAddressPanel("4");
	enterNumber(std::to_string(navgrid.width));
	setComputerAddressPanel("ENTER");

	// Set orientation
	setComputerAddressPanel("CLEAR");
	setComputerAddressPanel("8");
	enterNumber(std::to_string(navgrid.orientation));
	setComputerAddressPanel("ENTER");
}

namespace Tomcat
{
	UsableWaypoints Tomcat::StringToWP(const std::string& wp)
	{
		if (wp == "WP1")
			return UsableWaypoints::WP1;
		else if (wp == "WP2")
			return UsableWaypoints::WP2;
		else if (wp == "WP3")
			return UsableWaypoints::WP3;
		else if (wp == "FP")
			return UsableWaypoints::FP;
		else if (wp == "IP")
			return UsableWaypoints::IP;
		else if (wp == "ST")
			return UsableWaypoints::ST;
		else if (wp == "HA")
			return UsableWaypoints::HA;
		else if (wp == "DP")
			return UsableWaypoints::DP;
		else if (wp == "HB")
			return UsableWaypoints::HB;
		return UsableWaypoints::Error;
	}

	std::string WPToSring(const UsableWaypoints& wp)
	{
		if (wp == UsableWaypoints::WP1)
			return "WP1";
		else if (wp == UsableWaypoints::WP2)
			return "WP2";
		else if (wp == UsableWaypoints::WP3)
			return "WP3";
		else if (wp == UsableWaypoints::FP)
			return "FP";
		else if (wp == UsableWaypoints::IP)
			return "IP";
		else if (wp == UsableWaypoints::ST)
			return "ST";
		else if (wp == UsableWaypoints::HA)
			return "HA";
		else if (wp == UsableWaypoints::DP)
			return "DP";
		else if (wp == UsableWaypoints::HB)
			return "HB";
		return "Error";
	}
}