#include "HornetWindow.h"

HornetWindow::HornetWindow(QWidget* parent, const ConnectorCreator& connector, DTCLogger* log, DTCSettings* settings)
	: QWidget(parent), m_logger(log), m_settings(settings), m_connector(HornetConnector(connector))
{
	m_logger->debug("Creating HornetWindow...");
	m_ui.setupUi(this);

	radioPresets = { m_ui.line1, m_ui.line2, m_ui.line3, m_ui.line4, m_ui.line5, m_ui.line6, m_ui.line7, m_ui.line8, m_ui.line9, m_ui.line10, m_ui.line11, m_ui.line12, m_ui.line13, m_ui.line14, m_ui.line15, m_ui.line16, m_ui.line17, m_ui.line18, m_ui.line19, m_ui.line20, m_ui.lineM, m_ui.lineG,
					 m_ui.line1_2, m_ui.line2_2, m_ui.line3_2, m_ui.line4_2, m_ui.line5_2, m_ui.line6_2, m_ui.line7_2, m_ui.line8, m_ui.line9_2, m_ui.line10_2, m_ui.line11_2, m_ui.line12_2, m_ui.line13_2, m_ui.line14_2, m_ui.line15_2, m_ui.line16_2, m_ui.line17_2, m_ui.line18_2, m_ui.line19_2, m_ui.line20_2, m_ui.lineM_2, m_ui.lineG_2 };

	m_dialog = new WaypointAddDialog(nullptr, "HORNET", m_logger, m_settings);

	m_logger->debug("Linking Hornet UI...");
	connect(m_ui.AddWptButton, SIGNAL(clicked()), this, SLOT(OpenWaypointDialog()));
	connect(m_ui.RemoveWptButton, SIGNAL(clicked()), this, SLOT(RemoveWaypoint()));
	connect(m_ui.ModityWptButton, SIGNAL(clicked()), this, SLOT(ModifyWaypoint()));
	//connect(m_ui.ExportButtonToAC, SIGNAL(clicked()), this, SLOT(ExportToAircraft()));
	connect(m_ui.ExportButtonToFile, SIGNAL(clicked()), this, SLOT(ExportToFile()));
	connect(m_ui.ImportButton, SIGNAL(clicked()), this, SLOT(ImportFromFile()));
	connect(m_ui.ImportCFButton, SIGNAL(clicked()), this, SLOT(OpenSelectFileDiaglogCF()));

	m_logger->debug("Linking WaypointEditor UI...");
	connect(m_dialog->m_ui.Ok, SIGNAL(clicked()), this, SLOT(AddWaypoint()));
	connect(m_dialog->m_ui.Cancel, SIGNAL(clicked()), this, SLOT(CancelAdd()));
}

HornetWindow::~HornetWindow()
{
}

void HornetWindow::OpenWaypointDialog()
{
	if (m_waypointListItems.size() == m_connector.GetMaxWaypoints()) {
		m_logger->warning("All possible waypoints entered.");
		DTCUtils::OpenErrorBox("You have already entered all possible waypoints for the Hornet.");
		return;
	}

	m_dialog->show();
}

void HornetWindow::AddWaypoint()
{
	std::string latlong = m_dialog->m_ui.Position->text().toStdString();
	std::string name = m_dialog->m_ui.Name->text().toStdString();
	std::string alt = m_dialog->m_ui.Altitude->text().toStdString();
	unsigned int wpt = m_dialog->m_ui.WP->currentText().toInt();
	m_logger->info("Adding waypoint " + m_dialog->m_ui.WP->currentText().toStdString() + " NAME:" + name + " COORDS:" + latlong + " ALT:" + alt);

	m_waypoints[wpt] = m_dialog->m_ui.WP->currentText().toStdString() + " " + latlong + " " + alt + " " + name;
	m_waypointListItems.push_back(new QListWidgetItem((m_dialog->m_ui.WP->currentText().toStdString() + " - " + name).c_str()));
	m_ui.WaypointstList->addItem(m_waypointListItems[m_waypointListItems.size() - 1]);

	m_dialog->hide();
	m_dialog->m_ui.Position->clear();
	m_dialog->m_ui.Name->clear();
	m_dialog->m_ui.Altitude->clear();
	m_dialog->m_ui.WP->removeItem(m_dialog->m_ui.WP->findText(m_dialog->m_ui.WP->currentText()));

	if (m_waypointListItems.size() < m_connector.GetMaxWaypoints()) {
		if (m_flightData.size() != 0) {
			m_flightData.erase(name);
			AddCFWaypoints();
		}
	}
	else
		OpenWaypointDialog(); // Opening MessageBox error
}

void HornetWindow::RemoveWaypoint()
{
	auto selected = m_ui.WaypointstList->selectedItems();
	for (auto item : selected) {
		std::string wp = item->text().toStdString().substr(0, item->text().toStdString().find_first_of(" "));
		m_waypointListItems.erase(std::remove(m_waypointListItems.begin(), m_waypointListItems.end(), item), m_waypointListItems.end());
		m_waypoints.erase(atoi(wp.c_str()));
		m_dialog->m_ui.WP->addItem(wp.c_str());

		m_logger->info("Waypoint " + wp + " deleted.");
	}
	qDeleteAll(selected);
}

void HornetWindow::ModifyWaypoint()
{
	auto selected = m_ui.WaypointstList->selectedItems();
	for (auto item : selected) {
		// Set dialog Window
		unsigned int wp = atoi(item->text().toStdString().substr(0, item->text().toStdString().find_first_of(" ")).c_str());

		std::string line = m_waypoints[wp];
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
		m_waypoints.erase(wp);
		m_dialog->m_ui.WP->addItem(std::to_string(wp).c_str());
		m_dialog->m_ui.WP->setCurrentText(infos[0].c_str());
		m_logger->info("Waypoint " + infos[0] + " modified.");
	}
	qDeleteAll(selected);

	// Open dialog for modification and recreation
	OpenWaypointDialog();
}

void HornetWindow::CancelAdd()
{
	m_logger->info("Canceling waypoint add.");
	std::string name = m_dialog->m_ui.Name->text().toStdString();
	m_dialog->m_ui.Position->setText("");
	m_dialog->m_ui.Altitude->setText("");
	m_dialog->m_ui.Name->setText("");
	m_dialog->hide();

	if (m_waypointListItems.size() < m_connector.GetMaxWaypoints()) {
		if (m_flightData.size() != 0) {
			m_flightData.erase(name);
			AddCFWaypoints();
		}
	}
	else
		OpenWaypointDialog();
}

/*
void HornetWindow::ExportToAircraft()
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
}*/

void HornetWindow::ExportToFile()
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
	root.append_attribute("name") = "F18";

	// ADD IFF //
	auto IFF = root.append_child("IFF");
	IFF.append_attribute("M0") = std::to_string(m_ui.IFF_1_1->value() * 10 + m_ui.IFF_1_2->value()).c_str();
	IFF.append_attribute("M3") = std::to_string(m_ui.IFF_3_1->value() * 1000 + m_ui.IFF_3_2->value() * 100 + m_ui.IFF_3_3->value() * 10 + m_ui.IFF_3_4->value()).c_str();

	// ADD FUEL INFOS//
	auto bingo = root.append_child("Fuel");
	bingo.append_attribute("bingo") = std::to_string(m_ui.Bingo->value()).c_str();
	
	// ADD RADIO PRESETS //
	auto preset1 = root.append_child("PresetRAD1");
	for (size_t i = 0; i < 22; i++)
		preset1.append_attribute(std::string(1, (char)(i + 65)).c_str()) = radioPresets[i]->text().toStdString().c_str();

	auto preset2 = root.append_child("PresetRAD2");
	for (size_t i = 22; i < 44; i++)
		preset2.append_attribute(std::string(1, (char)(i + 65 - 22)).c_str()) = radioPresets[i]->text().toStdString().c_str();

	// ADD WAYPOINTS //
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

void HornetWindow::ImportFromFile()
{
	std::string fileName = QFileDialog::getOpenFileName(this, tr("Open DTC file"), "", tr("Xml Files (*.xml)")).toStdString();

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fileName.c_str(), pugi::parse_default | pugi::parse_declaration);
	if (!result) {
		DTCUtils::OpenErrorBox("ERROR : Can't open xml file ! File : " + fileName);
		m_logger->warning("Failed to open selected file : " + fileName + " ! Canceling...");
		return;
	}

	// Get root and check if aircraft is good
	pugi::xml_node root = doc.document_element();
	if (std::string(root.attribute("name").value()) != "F18") {
		DTCUtils::OpenErrorBox("This file is not for the hornet ! You can't open it !");
		m_logger->warning("This file is not for the hornet ! You can't open it !");
		return;
	}

	// Radio Pesets
	auto preset1 = root.select_single_node("PresetRAD1");
	for (size_t i = 0; i < 22; i++) {
		std::string letter = std::string(1, (char)(i + 65));
		std::string set = preset1.node().attribute(letter.c_str()).as_string();
		radioPresets[i]->setText(set.c_str());
	}

	auto preset2 = root.select_single_node("PresetRAD2");
	for (size_t i = 22; i < 44; i++) {
		std::string letter = std::string(1, (char)(i + 65 - 22));
		std::string set = preset2.node().attribute(letter.c_str()).as_string();
		radioPresets[i]->setText(set.c_str());
	}

	// Waypoints
	pugi::xpath_node masterWpts = root.select_single_node("Waypoints");
	pugi::xpath_node_set wpts = masterWpts.node().select_nodes("Waypoint");
	for (auto wpt : wpts) {
		std::string type = wpt.node().attribute("type").as_string();
		std::string pos = wpt.node().attribute("coordinates").as_string();;
		std::string alt = wpt.node().attribute("altitude").as_string();;
		std::string name = wpt.node().attribute("name").as_string();;

		unsigned int wpt = atoi(type.c_str());
		m_logger->info("Importing waypoint " + m_dialog->m_ui.WP->currentText().toStdString() + " NAME:" + name + " COORDS:" + pos + " ALT:" + alt);

		m_waypoints[wpt] = type + " " + pos + " " + alt + " " + name;
		m_waypointListItems.push_back(new QListWidgetItem((type + " - " + name).c_str()));
		m_ui.WaypointstList->addItem(m_waypointListItems[m_waypointListItems.size() - 1]);
		m_dialog->m_ui.WP->removeItem(m_dialog->m_ui.WP->findText(type.c_str()));
	}

	// IFF
	pugi::xpath_node IFF = root.select_single_node("IFF");
	pugi::xml_attribute attr;
	m_logger->info("Importing navgrid...");

	if (attr = IFF.node().attribute("M0")) {
		m_ui.IFF_1_1->setValue(attr.as_string()[0] - 48);
		m_ui.IFF_1_2->setValue(attr.as_string()[1] - 48);
	}
	if (attr = IFF.node().attribute("M3")) {
		m_ui.IFF_3_1->setValue(attr.as_string()[0] - 48);
		m_ui.IFF_3_2->setValue(attr.as_string()[1] - 48);
		m_ui.IFF_3_3->setValue(attr.as_string()[2] - 48);
		m_ui.IFF_3_4->setValue(attr.as_string()[3] - 48);
	}

	pugi::xpath_node Fuel = root.select_single_node("Fuel");
	m_logger->info("Importing fuel parameters...");

	if (attr = Fuel.node().attribute("bingo"))
		m_ui.Bingo->setValue(attr.as_int());
}

void HornetWindow::OpenSelectFileDiaglogCF()
{
	// Select cf file
	m_selectWindow = new FileSelectorWindow(nullptr, "cf", m_logger);
	connect(m_selectWindow->m_ui.OKButton, SIGNAL(clicked()), this, SLOT(ImportFromCombatFilte()));
	m_selectWindow->show();
}

void HornetWindow::ImportFromCombatFilte()
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
																m_flightData[wptName]["LONGITUDE"] = DTCUtils::decimalToLatLong(reader.readElementText().toStdString(), 4, false);
															else if (reader.name() == tr("Lat"))
																m_flightData[wptName]["LATTITUDE"] = DTCUtils::decimalToLatLong(reader.readElementText().toStdString(), 4, true);
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

void HornetWindow::AddCFWaypoints()
{
	for (auto wpt : m_flightData) {
		m_dialog->m_ui.Position->setText((wpt.second["LATTITUDE"] + wpt.second["LONGITUDE"]).c_str());
		m_dialog->m_ui.Name->setText(wpt.second["NAME"].c_str());
		m_dialog->m_ui.Altitude->setText(wpt.second["ALTITUDE"].c_str());
		OpenWaypointDialog();
	}
}
