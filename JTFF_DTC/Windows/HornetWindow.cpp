#include "HornetWindow.h"

HornetWindow::HornetWindow(QWidget* parent, const ConnectorCreator& connector, DTCLogger* log, DTCSettings* settings)
	: QWidget(parent), m_logger(log), m_settings(settings)//, m_connector(HornetConnector(connector))
{
	/*m_logger->debug("Creating HornetWindow...");
	m_ui.setupUi(this);

	m_dialog = new WaypointAddDialog(nullptr, "HORNET", m_logger, m_settings);

	m_logger->debug("Linking Hornet UI...");
	connect(m_ui.AddWptButton, SIGNAL(clicked()), this, SLOT(OpenWaypointDialog()));
	connect(m_ui.RemoveWptButton, SIGNAL(clicked()), this, SLOT(RemoveWaypoint()));
	connect(m_ui.ModityWptButton, SIGNAL(clicked()), this, SLOT(ModifyWaypoint()));
	connect(m_ui.ExportButtonToAC, SIGNAL(clicked()), this, SLOT(ExportToAircraft()));
	connect(m_ui.ExportButtonToFile, SIGNAL(clicked()), this, SLOT(ExportToFile()));
	connect(m_ui.ImportButton, SIGNAL(clicked()), this, SLOT(ImportFromFile()));
	connect(m_ui.ImportCFButton, SIGNAL(clicked()), this, SLOT(OpenSelectFileDiaglogCF()));

	m_logger->debug("Linking WaypointEditor UI...");
	connect(m_dialog->m_ui.Ok, SIGNAL(clicked()), this, SLOT(AddWaypoint()));
	connect(m_dialog->m_ui.Cancel, SIGNAL(clicked()), this, SLOT(CancelAdd()));*/
}

HornetWindow::~HornetWindow()
{
}


/*void HornetWindow::OpenWaypointDialog()
{
	m_dialog->show();
}

void HornetWindow::AddWaypoint()
{
	std::string latlong = m_dialog->m_ui.Position->text().toStdString();
	std::string name = m_dialog->m_ui.Name->text().toStdString();
	std::string alt = m_dialog->m_ui.Altitude->text().toStdString();
	auto wpt = Tomcat::StringToWP(m_dialog->m_ui.WP->currentText().toStdString());
	m_logger->info("Adding waypoint " + m_dialog->m_ui.WP->currentText().toStdString() + " NAME:" + name + " COORDS:" + latlong + " ALT:" + alt);

	m_waypoints[wpt] = m_dialog->m_ui.WP->currentText().toStdString() + " " + latlong + " " + alt + " " + name;
	m_waypointListItems.push_back(new QListWidgetItem((m_dialog->m_ui.WP->currentText().toStdString() + " - " + name).c_str()));
	m_ui.WaypointstList->addItem(m_waypointListItems[m_waypointListItems.size() - 1]);

	m_dialog->hide();
	m_dialog->m_ui.Position->clear();
	m_dialog->m_ui.Name->clear();
	m_dialog->m_ui.Altitude->clear();
	m_dialog->m_ui.WP->removeItem(m_dialog->m_ui.WP->findText(m_dialog->m_ui.WP->currentText()));

	if (m_flightData.size() != 0 && m_waypointListItems.size() < 9) {
		m_flightData.erase(name);
		AddCFWaypoints();
	}
}

void HornetWindow::RemoveWaypoint()
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

void HornetWindow::ModifyWaypoint()
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
}

void HornetWindow::ExportToFile()
{
	std::string fileName = QFileDialog::getSaveFileName(this, tr("Select path for file"), "", tr("Text Files (*.txt)")).toStdString();
	if (fileName == "")
		return;

	std::fstream file;
	file.open(fileName, std::ios::in | std::ios::out | std::ios::trunc);

	for (auto item : m_waypoints) {
		std::string line = item.second + "\n";
		file << line;
	}

	if (m_ui.NG_Enable->isChecked()) {
		std::string line = "NAVGRID " + m_ui.NG_LatLong->text().toStdString() + " " + m_ui.NG_Bearing->text().toStdString() + " " + m_ui.NG_Width->text().toStdString() + " " + std::to_string(m_ui.NG_Sectors->value());
		file << line;
	}

	file.close();
}

void HornetWindow::ImportFromFile()
{
	std::string fileName = QFileDialog::getOpenFileName(this, tr("Open DTC file"), "", tr("Text Files (*.txt)")).toStdString();

	std::ifstream file;
	file.open(fileName);

	std::string line;
	while (getline(file, line)) {
		bool isNAVGRID = false;
		if (line._Starts_with("NAVGRID")) {
			isNAVGRID = true;
			line = line.substr(line.find_first_of(' ') + 1, line.size() - line.find_first_of(' ') - 1);
		}

		// Split line to get infos
		std::vector<std::string> infos;
		size_t pos = 0;
		while ((pos = line.find(' ')) != std::string::npos) {
			infos.push_back(line.substr(0, pos));
			line.erase(0, pos + 1);
		}
		infos.push_back(line);

		if (!isNAVGRID) {
			// Use infos vector to add wpts
			Tomcat::UsableWaypoints wpt = Tomcat::StringToWP(infos[0]);
			m_logger->info("Adding waypoint " + m_dialog->m_ui.WP->currentText().toStdString() + " NAME:" + infos[3] + " COORDS:" + infos[1] + " ALT:" + infos[2]);

			m_waypoints[wpt] = infos[0] + " " + infos[1] + " " + infos[2] + " " + infos[4];
			m_waypointListItems.push_back(new QListWidgetItem((infos[0] + " - " + infos[4]).c_str()));
			m_ui.WaypointstList->addItem(m_waypointListItems[m_waypointListItems.size() - 1]);
			m_dialog->m_ui.WP->removeItem(m_dialog->m_ui.WP->findText(infos[0].c_str()));
		}
		else {
			// Use infos to add navgrid
			m_ui.NG_Enable->setChecked(true);
			m_ui.NG_LatLong->setText(infos[0].c_str());
			m_ui.NG_Bearing->setValue(atoi(infos[1].c_str()));
			m_ui.NG_Width->setValue(atoi(infos[2].c_str()));
			m_ui.NG_Sectors->setValue(atoi(infos[3].c_str()));
		}
	}

	file.close();
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
																m_flightData[wptName]["LONGITUDE"] = reader.readElementText().toStdString();
															else if (reader.name() == tr("Lat"))
																m_flightData[wptName]["LATTITUDE"] = reader.readElementText().toStdString();
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

	// Add all of that points to list
	AddCFWaypoints();
}

void HornetWindow::CancelAdd()
{
	m_logger->info("Canceling waypoint add.");
	std::string name = m_dialog->m_ui.Name->text().toStdString();
	if (m_flightData.size() != 0 && m_waypointListItems.size() < 9) {
		m_flightData.erase(name);
		if (m_flightData.size() != 0)
			AddCFWaypoints();
		else
			m_dialog->hide();
	}
	else
		m_dialog->hide();
}

void HornetWindow::AddCFWaypoints()
{
	for (auto wpt : m_flightData) {
		m_dialog->m_ui.Position->setText(("N" + wpt.second["LATTITUDE"] + "E" + wpt.second["LONGITUDE"]).c_str());
		m_dialog->m_ui.Name->setText(wpt.second["NAME"].c_str());
		m_dialog->m_ui.Altitude->setText(wpt.second["ALTITUDE"].c_str());
		OpenWaypointDialog();
	}
}
*/