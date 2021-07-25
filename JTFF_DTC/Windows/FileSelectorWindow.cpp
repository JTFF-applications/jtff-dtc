#include "FileSelectorWindow.h"

FileSelectorWindow::FileSelectorWindow(QWidget *parent, const std::string& fileType, DTCLogger* logger)
	: QWidget(parent), m_fileType(fileType), m_logger(logger)
{
	m_ui.setupUi(this);
	setWindowTitle(("JTFF DTC - Select ." + fileType + " file").c_str());

	connect(m_ui.SelectButton, SIGNAL(clicked()), this, SLOT(SelectFile()));
}

FileSelectorWindow::~FileSelectorWindow()
{
}

void FileSelectorWindow::SelectFile()
{
	m_ui.FlightSelector->clear();

	m_logger->debug("Selecting CF file...");
	std::string fileName = QFileDialog::getOpenFileName(this, tr("Open .cf file"), "", tr("Combatfilte Files (*.cf)")).toStdString();
	m_ui.text->setText(fileName.c_str());

	if (!QFile::exists(fileName.c_str())) {
		m_logger->warning("File selected does not exist.");
		return;
	}

	// Get xml file and extract it
	libzippp::ZipArchive zf(fileName);
	zf.open(libzippp::ZipArchive::ReadOnly);

	libzippp::ZipEntry xmlFile = zf.getEntry("mission.xml");
	std::ofstream file("temp/mission.xml");
	xmlFile.readContent(file);
	file.close();

	zf.close();

	// Get all flights and show them in list
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
										m_ui.FlightSelector->addItem(reader.readElementText());
								else
									reader.skipCurrentElement();
						else
							reader.skipCurrentElement();
				else
					reader.skipCurrentElement();
		else
			m_logger->warning("Incorrect XML file detetcted ! Closing.");

	f.close();
}
