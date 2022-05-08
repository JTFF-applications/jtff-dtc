#include <QFileDialog>
#include <QMessageBox>

#include <pugixml.hpp>
#include <libzippp/libzippp.h>

#include <fstream>

#include "CombatFlite.h"

CombatFlite::CombatFlite()
	: m_logger((Logger::Get()))
{
}

void CombatFlite::RequestFile()
{
	std::string filePath = QFileDialog::getOpenFileName(nullptr, "Select path for file", "", "CombatFlight Files (*.cf)").toStdString();
	if (filePath.empty() || !std::filesystem::exists(filePath) || std::filesystem::path(filePath).extension() != ".cf")
	{
		m_logger->error("{} is not a valid cf file !", filePath);
		QMessageBox(QMessageBox::Icon::Critical, "Error", "Please select a valid cf file").exec();
		return;
	}

	libzippp::ZipArchive zf(filePath);
	zf.open(libzippp::ZipArchive::ReadOnly);

	libzippp::ZipEntry xmlFile = zf.getEntry("mission.xml");
	std::ofstream file("temp/mission.xml");
	if (!file)
	{
		m_logger->error("Could not open {} for writing", "temp/mission.xml");
		QMessageBox(QMessageBox::Icon::Critical, "Error", "Can't create file in app/temp. Please check if temp folder exists in main directory.").exec();
		return;
	}
	xmlFile.readContent(file);
	file.close();

	zf.close();

	if (!pugi::xml_document().load_file("temp/mission.xml"))
	{
		m_logger->error("{} is not  avalid cf file !", filePath);
		QMessageBox(QMessageBox::Icon::Critical, "Error", "Please select a valid or readable cf file.").exec();
		return;
	}

	m_logger->info("CF file selected : {}", filePath);
	m_path = "temp/mission.xml";
}

void CombatFlite::ReadFile()
{
	if (m_path.empty())
	{
		m_logger->error("CombatFlite::ReadFile called with empty cf file !");
		return;
	}

	RequestFlight();
}

void CombatFlite::RequestFlight()
{
	const std::list<std::string> flights = GetFlightsFromFile();
	const auto fn = [&]
	{
		m_selector.hide();
		m_flight = m_selector.GetSelected();
		m_waypoints = GetWaypointsFromFile();
		m_logger->info("Selected CF flight {}", m_flight);
		m_logger->info("Found {} waypoints", m_waypoints.size());
	};

	QObject::connect(m_selector.GetButton(), &QPushButton::clicked, fn);
	m_selector.SetFlights(flights);
	m_selector.exec();
}

const std::list<std::string> CombatFlite::GetFlightsFromFile()
{
	std::list<std::string> flights;

	pugi::xml_document doc;
	doc.load_file(m_path.c_str());

	pugi::xpath_node_set routes = doc.select_nodes("/Mission/Routes/Route");
	m_logger->info("Found {} routes", routes.size());
	for (pugi::xpath_node route : routes)
	{
		pugi::xpath_node name = route.node().select_node("Name");
		flights.push_back(name.node().child_value());
	}
	return flights;
}

std::list<Waypoint> CombatFlite::GetWaypointsFromFile()
{
	std::list<Waypoint> waypoints;

	pugi::xml_document doc;
	doc.load_file(m_path.c_str());

	pugi::xpath_node_set waypoints_node = doc.select_nodes(std::format("/Mission/Routes/Route[Name='{}']/Waypoints/Waypoint", m_flight).c_str());
	for (pugi::xpath_node waypoint : waypoints_node)
	{
		std::string lat = waypoint.node().select_node("Lat").node().child_value();
		std::string lon = waypoint.node().select_node("Lon").node().child_value();
		std::string alt = waypoint.node().select_node("Altitude").node().child_value();
		std::string name = waypoint.node().select_node("Name").node().child_value();

		Waypoint wp = Waypoint::FromDecimalCoordinates(lat, lon);
		wp.SetAltitude(std::atoi(alt.c_str()));
		wp.SetName(name);

		waypoints.push_back(wp);
	}
	return waypoints;
}
