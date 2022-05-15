#include <QMessageBox>

#include <pugixml.hpp>

#include "TomcatXmlParser.h"

TomcatXmlParser::TomcatXmlParser(const std::string& aircraft, const std::string& filePath)
	: BaseXmlParser(aircraft, filePath)
{
}

const std::list<std::pair<const std::string, const Waypoint>> TomcatXmlParser::GetWaypoints()
{
	pugi::xml_document doc;
	doc.load_file(m_file.c_str(), pugi::parse_default | pugi::parse_declaration);
	std::list<std::pair<const std::string, const Waypoint>> waypoints;

	pugi::xpath_node_set wpts = doc.select_nodes("/Aircraft/Waypoints/Waypoint");
	for (const auto& wpt : wpts)
	{
		std::string type = wpt.node().attribute("type").as_string();
		std::string lat = wpt.node().attribute("lattitude").as_string();
		std::string lon = wpt.node().attribute("longitude").as_string();
		std::string name = wpt.node().attribute("name").as_string();
		int alt = wpt.node().attribute("altitude").as_int();
		int spd = wpt.node().attribute("speed").as_int();
		int hdg = wpt.node().attribute("heading").as_int();

		try
		{
			Waypoint wpt = Waypoint::FromStringCoordinates(lat, lon);
			wpt.SetName(name);
			wpt.SetAltitude(alt);
			wpt.SetSpeed(spd);
			wpt.SetHeading(hdg);

			waypoints.push_back(std::make_pair(type, wpt));
		}
		catch (const Coordinate::bad_coordinate& except)
		{
			QMessageBox(QMessageBox::Icon::Warning, "XML Error", std::format("Failed to parse waypoint : {} !", name).c_str(), QMessageBox::Ok).exec();
			m_logger->error(std::format("Failed to parse waypoint : {} ! Error : {}.", name, except.what()));
		}
	}
	return waypoints;
}

void TomcatXmlParser::WriteWaypoints(const std::list<std::pair<const std::string, const Waypoint>> waypoints)
{
	pugi::xml_document doc;
	doc.load_file(m_file.c_str(), pugi::parse_default | pugi::parse_declaration);

	pugi::xml_node wpts = doc.document_element().append_child("Waypoints");
	for (const auto& wpt : waypoints)
	{
		pugi::xml_node wpt_node = wpts.append_child("Waypoint");
		wpt_node.append_attribute("type") = wpt.first.c_str();
		wpt_node.append_attribute("lattitude") = wpt.second.GetLattitude().c_str();
		wpt_node.append_attribute("longitude") = wpt.second.GetLongitude().c_str();
		wpt_node.append_attribute("name") = wpt.second.GetName().c_str();
		wpt_node.append_attribute("altitude") = wpt.second.GetAltitude();
		wpt_node.append_attribute("speed") = wpt.second.GetSpeed();
		wpt_node.append_attribute("heading") = wpt.second.GetHeading();
	}
	BaseXmlParser::SaveDocument(doc);
}

const Navgrid TomcatXmlParser::GetNavgrid()
{
	pugi::xml_document doc;
	doc.load_file(m_file.c_str(), pugi::parse_default | pugi::parse_declaration);

	pugi::xpath_node navgrid = doc.select_single_node("/Aircraft/Navgrid");
	bool isChecked = navgrid.node().attribute("enabled").as_bool();
	const std::string navgrid_lat = navgrid.node().attribute("lattitude").as_string();
	const std::string navgrid_lon = navgrid.node().attribute("longitude").as_string();
	const int navgrid_bearing = navgrid.node().attribute("bearing").as_int();
	const int navgrid_width = navgrid.node().attribute("width").as_int();
	const int navgrid_sectors = navgrid.node().attribute("sectors").as_int();

	return Navgrid(isChecked, navgrid_lat, navgrid_lon, navgrid_width, navgrid_bearing, navgrid_sectors);
}

void TomcatXmlParser::WriteNavgrid(const Navgrid navgrid)
{
	pugi::xml_document doc;
	doc.load_file(m_file.c_str(), pugi::parse_default | pugi::parse_declaration);

	pugi::xml_node navgrid_node = doc.document_element().append_child("Navgrid");
	navgrid_node.append_attribute("enabled") = navgrid.GetChecked();
	navgrid_node.append_attribute("lattitude") = navgrid.GetLattitude().c_str();
	navgrid_node.append_attribute("longitude") = navgrid.GetLongitude().c_str();
	navgrid_node.append_attribute("bearing") = navgrid.GetBearing();
	navgrid_node.append_attribute("width") = navgrid.GetWidth();
	navgrid_node.append_attribute("sectors") = navgrid.GetSectors();

	BaseXmlParser::SaveDocument(doc);
}
