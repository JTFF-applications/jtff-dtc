#ifndef DTCSETTINGS_H
#define DTCSETTINGS_H

#include <fstream>
#include <vector>

#include "DTCLogger.h"

#define JTFF_DTC_API __declspec(dllexport)

class JTFF_DTC_API DTCSettings
{
public:
	DTCSettings(DTCLogger* logger);
	~DTCSettings();

	std::string getParameter(const std::string& param);
	bool setParameter(const std::string& param, const std::string& value);
	
private:
	std::fstream m_file;

	DTCLogger* m_logger;
};

#endif //!DTCSETTINGS_H