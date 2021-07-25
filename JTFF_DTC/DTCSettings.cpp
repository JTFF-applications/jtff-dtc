#include "DTCSettings.h"

DTCSettings::DTCSettings(DTCLogger* logger)
	: m_logger(logger)
{
	m_file.open("settings.ini");
	if (!m_file.is_open())
		m_logger->critical("Can't open settings.ini.");
}

DTCSettings::~DTCSettings()
{
	m_file.close();
}

std::string DTCSettings::getParameter(const std::string& param)
{
	std::string line, res = "";
	while (std::getline(m_file, line)) {
		if (line.empty())
			continue;
		if (line._Starts_with(param))
			res = line.substr(param.size() + 1);
	}

	m_file.seekg(0, std::ios::beg);
	return res;
}

bool DTCSettings::setParameter(const std::string& param, const std::string& value)
{
	std::string line;
	std::vector<std::string> lines;
	int counter = 0;

	while (std::getline(m_file, line)) {
		counter++;
		lines.push_back(line);
		if (line._Starts_with(param))
			break;
		if (m_file.eof())
			return false;
	}

	// Cleaning file
	m_file.close();
	m_file.open("settings.ini", std::ios::out |std::ios::trunc);
	if (!m_file.is_open())
		m_logger->critical("Can't open settings.ini.");
	m_file.close();

	// Reopening file for writing new data
	m_file.open("settings.ini");
	if (!m_file.is_open())
		m_logger->critical("Can't open settings.ini.");
	for (std::string line : lines) {
		if (line._Starts_with(param))
			line = param + "=" + value;
		m_file << line;
	}

	m_file.seekg(0, std::ios::beg);
	return true;
}
