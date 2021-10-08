#ifndef TOMCATCONENCTOR_H
#define TOMCATCONENCTOR_H

#include <string>
#include <unordered_map>

#include <JTFF_DTC_API.h>

class TomcatConnector : protected BaseConnector
{
public:
	TomcatConnector(const ConnectorCreator& base);
	~TomcatConnector();

	void enterWaypoints(const std::unordered_map<Tomcat::UsableWaypoints, std::string>& waypoints);
	void enterNavgrid(const Tomcat::NavgridParams& navgrid);

private:
	void setComputerAddressPanel(const std::string& num);
	void setTargetInformationDisplay(const std::string& num);
	void enterNumber(const std::string& nb);
	void enterWaypoint(const std::string& coords, const Tomcat::UsableWaypoints& waypoint);

	std::unordered_map<std::string, std::string> m_capKeys = {
		{"0", "RIO_CAP_BRG_0"},
		{"1", "RIO_CAP_LAT_1" },
		{"2", "RIO_CAP_NBR_2" },
		{"3", "RIO_CAP_SPD_3" },
		{"4", "RIO_CAP_ALT_4" },
		{"5", "RIO_CAP_RNG_5" },
		{"6", "RIO_CAP_LONG_6" },
		{"7", "RIO_CAP_7" },
		{"8", "RIO_CAP_HDG_8" },
		{"9", "RIO_CAP_9" },
		{"N", "RIO_CAP_NE" },
		{"E", "RIO_CAP_NE" },
		{"S", "RIO_CAP_SW" },
		{"W", "RIO_CAP_SW" },
		{"CLEAR", "RIO_CAP_CLEAR"},
		{"ENTER", "RIO_CAP_ENTER"},

		{"CAP_1", "RIO_CAP_BTN_1"},
		{"CAP_2", "RIO_CAP_BTN_2"},
		{"CAP_3", "RIO_CAP_BTN_3"},
		{"CAP_4", "RIO_CAP_BTN_4"},
		{"CAP_5", "RIO_CAP_BTN_5"},
		{"CAP_6", "RIO_CAP_BTN_6"},
		{"CAP_7", "RIO_CAP_BTN_7"},
		{"CAP_8", "RIO_CAP_BTN_8"},
		{"CAP_9", "RIO_CAP_BTN_9"},
		{"CAP_10", "RIO_CAP_BTN_10"}
	};
};

#endif //!TOMCATCONENCTOR_H
