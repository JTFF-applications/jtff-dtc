#include "DTCLogger.h"

DTCLogger::DTCLogger(const std::string& fileName)
{
	m_file.open(fileName, std::ios::in | std::ios::app);

	if (!m_file.is_open())
		throw std::exception("Can't open or create log file !");

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string dateTime(30, '\0');
    std::strftime(&dateTime[0], dateTime.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    m_file << "---------------------- Log opened : " + dateTime + " ----------------------\n";
}

DTCLogger::~DTCLogger()
{
	m_file.close();
}

void DTCLogger::info(const std::string& msg)
{
    std::stringstream buf;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string dateTime(30, '\0');
    std::strftime(&dateTime[0], dateTime.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    buf << dateTime << " INFO" << std::setw(15) << msg << std::endl;
    m_file << buf.str();
}

void DTCLogger::debug(const std::string& msg)
{
    std::stringstream buf;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string dateTime(30, '\0');
    std::strftime(&dateTime[0], dateTime.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    buf << dateTime << " DEBUG" << std::setw(15) << msg << std::endl;
    m_file << buf.str();
}

void DTCLogger::warning(const std::string& msg)
{
    std::stringstream buf;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string dateTime(30, '\0');
    std::strftime(&dateTime[0], dateTime.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    buf << dateTime << " WARNING" << std::setw(15) << msg << std::endl;
    m_file << buf.str();
}

void DTCLogger::critical(const std::string& msg)
{
    std::stringstream buf;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string dateTime(30, '\0');
    std::strftime(&dateTime[0], dateTime.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    buf << dateTime << " CRITICAL" << std::setw(15) << msg << std::endl;
    m_file << buf.str();
    
    throw std::exception(msg.c_str());
}
