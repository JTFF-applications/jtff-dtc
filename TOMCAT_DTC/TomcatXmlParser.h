#pragma once

#include <Utilities/BaseXmlParser.h>

#include "Navgrid.h"

class TomcatXmlParser : public BaseXmlParser
{
public:
	TomcatXmlParser(const std::string& aircraft, const std::string& filePath = "");
	~TomcatXmlParser() = default;

	virtual const std::list<std::pair<const std::string, const Waypoint>> GetWaypoints() override;
	virtual void WriteWaypoints(const std::list<std::pair<const std::string, const Waypoint>> waypoints) override;

	const Navgrid GetNavgrid();
	void WriteNavgrid(const Navgrid navgrid);
};

