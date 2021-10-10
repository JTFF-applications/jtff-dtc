#include "TomcatWindow.h"

TomcatWindow::TomcatWindow(QWidget *parent, const ConnectorCreator& connector, DTCLogger* log, DTCSettings* settings)
	: QWidget(parent), m_logger(log), m_settings(settings), m_connector(TomcatConnector(connector))
{
	m_logger->debug("Creating TomcatWindow...");
	m_ui.setupUi(this);

	m_dialog = new WaypointAddDialog(nullptr, DTCUtils::Aircraft::Tomcat, m_logger, m_settings);

	m_logger->debug("Linking Tomcat UI...");
	connect(m_ui.AddWptButton, SIGNAL(clicked()), this, SLOT(OpenWaypointDialog()));
	connect(m_ui.RemoveWptButton, SIGNAL(clicked()), this, SLOT(RemoveWaypoint()));
	connect(m_ui.ModityWptButton, SIGNAL(clicked()), this, SLOT(ModifyWaypoint()));
	connect(m_ui.ExportButtonToAC, SIGNAL(clicked()), this, SLOT(ExportToAircraft()));
	connect(m_ui.ExportButtonToFile, SIGNAL(clicked()), this, SLOT(ExportToFile()));
	connect(m_ui.ImportButton, SIGNAL(clicked()), this, SLOT(ImportFromFile()));
	connect(m_ui.ImportCFButton, SIGNAL(clicked()), this, SLOT(OpenSelectFileDiaglogCF()));

	m_logger->debug("Linking WaypointEditor UI...");
	connect(m_dialog->m_ui.Ok, SIGNAL(clicked()), this, SLOT(AddWaypoint()));
	connect(m_dialog->m_ui.Cancel, SIGNAL(clicked()), this, SLOT(CancelAdd()));
}

TomcatWindow::~TomcatWindow()
{
	for (auto& item : m_waypointListItems)
		if (item)
			delete item;

	delete m_dialog;
}

void TomcatWindow::OpenWaypointDialog()
{
	if (m_waypointListItems.size() == 9) {
		m_logger->warning("All possible waypoints entered.");
		DTCUtils::OpenErrorBox("You have already entered all possible waypoints for the Tomcat.");
		return;
	}

	m_dialog->show();
}

void TomcatWindow::AddWaypoint()
{
	std::string latlong = m_dialog->m_ui.Position->text().toStdString();
	std::string name = m_dialog->m_ui.Name->text().toStdString();
	std::string alt = m_dialog->m_ui.Altitude->text().toStdString();
	Tomcat::UsableWaypoints wpt = Tomcat::StringToWP(m_dialog->m_ui.WP->currentText().toStdString());
	m_logger->info("Adding waypoint " + m_dialog->m_ui.WP->currentText().toStdString() + " NAME:" + name + " COORDS:" + latlong + " ALT:" + alt);

	m_waypoints[wpt] = m_dialog->m_ui.WP->currentText().toStdString() + " " + latlong + " " + alt + " " + name;
	m_waypointListItems.push_back(new QListWidgetItem((m_dialog->m_ui.WP->currentText().toStdString() + " - " + name).c_str()));
	m_ui.WaypointstList->addItem(m_waypointListItems[m_waypointListItems.size() - 1]);

	m_dialog->hide();
	m_dialog->m_ui.Position->clear();
	m_dialog->m_ui.Name->clear();
	m_dialog->m_ui.Altitude->clear();
	m_dialog->m_ui.WP->removeItem(m_dialog->m_ui.WP->findText(m_dialog->m_ui.WP->currentText()));

	if (m_waypointListItems.size() < 9) {
		if (m_flightData.size() != 0) {
			m_flightData.erase(name);
			AddCFWaypoints();
		}
	}
	else
		OpenWaypointDialog(); // Opening MessageBox error
}

void TomcatWindow::RemoveWaypoint()
{
	auto selected = m_ui.WaypointstList->selectedItems();
	for (auto item : selected) {
		std::string wp = item->text().toStdString().substr(0, item->text().toStdString().find_first_of(" "));
		m_waypointListItems.erase(std::remove(m_waypointListItems.begin(), m_waypointListItems.end(), item), m_waypointListItems.end());
		m_waypoints.erase(Tomcat::StringToWP(wp));
		m_dialog->m_ui.WP->addItem(wp.c_str());

		m_logger->info("Waypoint " + wp + " deleted.");
	}
	qDeleteAll(selected);
}

void TomcatWindow::ModifyWaypoint()
{ 
	auto selected = m_ui.WaypointstList->selectedItems();
	for (auto item : selected) {
		// Set dialog Window
		std::string wp = item->text().toStdString().substr(0, item->text().toStdString().find_first_of(" "));

		std::string line = m_waypoints[Tomcat::StringToWP(wp)];
		std::vector<std::string> infos;
		size_t pos = 0;
		while ((pos = line.find(' ')) != std::string::npos) {
			infos.push_back(line.substr(0, pos));
			line.erase(0, pos + 1);
		}
		infos.push_back(line);

		m_dialog->m_ui.Position->setText(infos[1].c_str());
		m_dialog->m_ui.Altitude->setText(infos[2].c_str());
		m_dialog->m_ui.Name->setText(infos[3].c_str());

		// Delete waypoint
		m_waypointListItems.erase(std::remove(m_waypointListItems.begin(), m_waypointListItems.end(), item), m_waypointListItems.end());
		m_waypoints.erase(Tomcat::StringToWP(wp));
		m_dialog->m_ui.WP->addItem(wp.c_str());
		m_dialog->m_ui.WP->setCurrentText(infos[0].c_str());
		m_logger->info("Waypoint " + infos[0] + " modified.");
	}
	qDeleteAll(selected);

	// Open dialog for modification and recreation
	OpenWaypointDialog();
}

void TomcatWindow::ExportToAircraft()
{
	Tomcat::NavgridParams navgrid;
	if (m_ui.NG_Enable->isChecked()) {
		navgrid.center = m_ui.NG_LatLong->text().toStdString();
		navgrid.orientation = m_ui.NG_Bearing->text().toInt();
		navgrid.width = m_ui.NG_Width->text().toInt();
		navgrid.sectors = m_ui.NG_Sectors->value();
	}
	m_logger->debug("Entering waypoints and navgrid (if checked)");

	if (m_ui.NG_Enable->isChecked())
		m_connector.enterNavgrid(navgrid);
	m_connector.enterWaypoints(m_waypoints);
}

void TomcatWindow::ExportToFile()
{
	pugi::xml_document doc;
	auto declarationNode = doc.append_child(pugi::node_declaration);
	declarationNode.append_attribute("version") = "1.0";
	declarationNode.append_attribute("encoding") = "ISO-8859-1";
	declarationNode.append_attribute("standalone") = "yes";

	std::string fileName = QFileDialog::getSaveFileName(this, tr("Select path for file"), "", tr("Xml Files (*.xml)")).toStdString();
	if (fileName == "")
		return;

	auto root = doc.append_child("Aircraft");
	root.append_attribute("name") = "F14";

	auto navgrid = root.append_child("Navgrid");
	navgrid.append_attribute("enabled") = m_ui.NG_Enable->isChecked() ? "true" : "false";
	navgrid.append_attribute("center") = m_ui.NG_LatLong->text().toStdString().c_str();
	navgrid.append_attribute("bearing") = m_ui.NG_Bearing->text().toStdString().c_str();
	navgrid.append_attribute("width") = m_ui.NG_Width->text().toStdString().c_str();
	navgrid.append_attribute("sectors") = std::to_string(m_ui.NG_Sectors->value()).c_str();

	auto wpts = root.append_child("Waypoints");
	for (auto item : m_waypoints) {
		std::vector<std::string> infos;
		size_t pos = 0;
		while ((pos = item.second.find(' ')) != std::string::npos) {
			infos.push_back(item.second.substr(0, pos));
			item.second.erase(0, pos + 1);
		}
		infos.push_back(item.second);

		auto wpt = wpts.append_child("Waypoint");
		wpt.append_attribute("type") = infos[0].c_str();
		wpt.append_attribute("coordinates") = infos[1].c_str();
		wpt.append_attribute("altitude") = infos[2].c_str();
		wpt.append_attribute("name") = infos[3].c_str();
	}

	bool saveSucceeded = doc.save_file(fileName.c_str(), PUGIXML_TEXT("  "));
	if (!saveSucceeded)
		m_logger->warning("Can't save xml file, check permissions ! This will not be saved !");
}

void TomcatWindow::ImportFromFile()
{
	std::string fileName = QFileDialog::getOpenFileName(this, tr("Open DTC file"), "", tr("Xml Files (*.xml)")).toStdString();
	if (fileName.empty())
		return;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str(), pugi::parse_default | pugi::parse_declaration);
	if (!result) {
		DTCUtils::OpenErrorBox("ERROR : Can't open xml file ! File:" + fileName);
		m_logger->warning("Failed to open selected file : " + fileName + " ! Canceling...");
		return;
	}

	// Get root and check if aircraft is good
	pugi::xml_node root = doc.document_element();
	if (std::string(root.attribute("name").value()) != "F14") {
		DTCUtils::OpenErrorBox("This file is not for the tomcat ! You can't open it !");
		m_logger->warning("This file is not for the tomcat ! You can't open it !");
		return;
	}

	// Waypoints
	pugi::xpath_node masterWpts = root.select_single_node("Waypoints");
	pugi::xpath_node_set wpts = masterWpts.node().select_nodes("Waypoint");
	for (auto wpt : wpts) {
		std::string type = wpt.node().attribute("type").as_string();
		std::string pos = wpt.node().attribute("coordinates").as_string();;
		std::string alt = wpt.node().attribute("altitude").as_string();;
		std::string name = wpt.node().attribute("name").as_string();;

		Tomcat::UsableWaypoints wpt = Tomcat::StringToWP(type);
		m_logger->info("Importing waypoint " + m_dialog->m_ui.WP->currentText().toStdString() + " NAME:" + name + " COORDS:" + pos + " ALT:" + alt);

		m_waypoints[wpt] = type + " " + pos + " " + alt + " " + name;
		m_waypointListItems.push_back(new QListWidgetItem((type + " - " + name).c_str()));
		m_ui.WaypointstList->addItem(m_waypointListItems[m_waypointListItems.size() - 1]);
		m_dialog->m_ui.WP->removeItem(m_dialog->m_ui.WP->findText(type.c_str()));
	}

	// Navgrid
	pugi::xpath_node navgrid = root.select_single_node("Navgrid");
	if (navgrid) {
		pugi::xml_attribute attr;
		m_logger->info("Importing navgrid...");

		if (attr = navgrid.node().attribute("enabled"))
			m_ui.NG_Enable->setChecked(attr.as_bool());
		if (attr = navgrid.node().attribute("center"))
			m_ui.NG_LatLong->setText(attr.as_string());
		if (attr = navgrid.node().attribute("bearing"))
			m_ui.NG_Bearing->setValue(attr.as_int());
		if (attr = navgrid.node().attribute("width"))
			m_ui.NG_Width->setValue(attr.as_int());
		if (attr = navgrid.node().attribute("sectors"))
			m_ui.NG_Sectors->setValue(attr.as_int());
	}
}

void TomcatWindow::OpenSelectFileDiaglogCF()
{
	// Select cf file
	m_selectWindow = new FileSelectorWindow(nullptr, "cf", m_logger);
	connect(m_selectWindow->m_ui.OKButton, SIGNAL(clicked()), this, SLOT(ImportFromCombatFilte()));
	m_selectWindow->show();
}

void TomcatWindow::ImportFromCombatFilte()
{
	// Get .cf path and selected filght, delete window
	std::string filePath = m_selectWindow->m_ui.text->text().toStdString();
	std::string selectedFlight = m_selectWindow->m_ui.FlightSelector->currentText().toStdString();
	m_selectWindow->hide();
	delete m_selectWindow;

	// Get all flight waypoints

	QFile f("temp/mission.xml");
	if (!f.open(QIODevice::ReadOnly))
	{
		m_logger->warning("Error while loading file");
		return;
	}

	QXmlStreamReader reader(&f);
	if (reader.readNextStartElement())
		if (reader.name() == tr("Mission"))
			while (reader.readNextStartElement())
				if (reader.name() == tr("Routes"))
					while (reader.readNextStartElement())
						if (reader.name() == tr("Route"))
							while (reader.readNextStartElement())
								if (reader.name() == tr("Name"))
									if (reader.readElementText() == selectedFlight.c_str())
										while (reader.readNextStartElement())
											if (reader.name() == tr("Waypoints"))
												while (reader.readNextStartElement())
													if (reader.name() == tr("Waypoint")) {
														std::string wptName;
														while (reader.readNext())
															if (reader.name() == tr("Name")) {
																wptName = reader.readElementText().toStdString();
																wptName.erase(remove(wptName.begin(), wptName.end(), ' '), wptName.end());
																m_flightData[wptName]["NAME"] = wptName;
															}
															else if (reader.name() == tr("Lon"))
																m_flightData[wptName]["LONGITUDE"] = DTCUtils::decimalToLatLong(reader.readElementText().toStdString(), 1, false);
															else if (reader.name() == tr("Lat"))
																m_flightData[wptName]["LATTITUDE"] = DTCUtils::decimalToLatLong(reader.readElementText().toStdString(), 1, true);
															else if (reader.name() == tr("Altitude"))
																m_flightData[wptName]["ALTITUDE"] = reader.readElementText().toStdString();
															else {
																if (reader.name().toString().toStdString() == "Route")
																	break;
																continue;
															}
													}
													else
														reader.skipCurrentElement();											
											else
												reader.skipCurrentElement();
									else
										continue;
								else
									reader.skipCurrentElement();
						else
							reader.skipCurrentElement();
				else
					reader.skipCurrentElement();
		else
			reader.skipCurrentElement();
	else
		m_logger->warning("Incorrect XML file detetcted ! Closing.");

	f.close();

	m_logger->debug("CF File read complete !");

	// Add all of that points to list
	AddCFWaypoints();
}

void TomcatWindow::CancelAdd()
{
	m_logger->info("Canceling waypoint add.");
	std::string name = m_dialog->m_ui.Name->text().toStdString();
	m_dialog->m_ui.Position->setText("");
	m_dialog->m_ui.Altitude->setText("");
	m_dialog->m_ui.Name->setText("");
	m_dialog->hide();

	if (m_waypointListItems.size() < 9) {
		if (m_flightData.size() != 0) {
			m_flightData.erase(name);
			AddCFWaypoints();
		}
	}
	else
		OpenWaypointDialog();
}

void TomcatWindow::AddCFWaypoints()
{
	for (auto wpt : m_flightData) {
		m_dialog->m_ui.Position->setText((wpt.second["LATTITUDE"] + wpt.second["LONGITUDE"]).c_str());
		m_dialog->m_ui.Name->setText(wpt.second["NAME"].c_str());
		m_dialog->m_ui.Altitude->setText(wpt.second["ALTITUDE"].c_str());
		OpenWaypointDialog();
	}
}
