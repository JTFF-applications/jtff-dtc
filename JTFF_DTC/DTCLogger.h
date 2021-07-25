#ifndef DTCLOGGER_H
#define DTCLOGGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

class DTCLogger
{
public:
	DTCLogger(const std::string& fileName);
	~DTCLogger();

	void info(const std::string& msg);
	void debug(const std::string& msg);
	void warning(const std::string& msg);
	void critical(const std::string& msg);

private:
	std::fstream m_file;
};

#endif //!LOGGER_H