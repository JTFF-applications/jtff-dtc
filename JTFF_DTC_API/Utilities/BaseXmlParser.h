#pragma once

#include "Waypoint.h"
#include "../Logger.h"

class JTFF_DTC_API_EXPORT BaseXmlParser
{
public:
	enum class ERROR_TYPE
	{
		BAD_FILE,
		BAD_AIRCRAFT
	};

	class JTFF_DTC_API_EXPORT bad_parser : public std::exception
	{
	public:
		bad_parser(const ERROR_TYPE& error, const char* file)
			: m_error(error), m_file(file)
		{
		}

		virtual const char* what() const throw() { return m_file; }
		const ERROR_TYPE error() const throw () { return m_error; }

	private:
		const ERROR_TYPE m_error;
		const char* m_file;
	};

public:
	BaseXmlParser(const std::string& aircraft, const std::string& filePath = "");
	~BaseXmlParser();

	virtual void SaveDocument(const pugi::xml_document& doc);
	virtual const std::list<std::pair<const std::string, const Waypoint>> GetWaypoints() = 0;
	virtual void WriteWaypoints(const std::list<std::pair<const std::string, const Waypoint>> waypoints) = 0;

protected:
	std::shared_ptr<spdlog::logger> m_logger;
	const std::string m_aircraft;
	std::string m_file;
};
