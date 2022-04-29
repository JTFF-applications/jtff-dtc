#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <memory>

#include "jtff_dtc_api_global.h"

class JTFF_DTC_API_EXPORT Logger
{
public:
	Logger()
		: m_logger(spdlog::basic_logger_mt("logger", "logs/dtc.log"))
	{}

	Logger(Logger& other) = delete;
	void operator=(const Logger&) = delete;

	static std::shared_ptr<spdlog::logger> Get()
	{
		if (!m_instance)
			m_instance = new Logger();
		return m_instance->m_logger;
	}

private:
	static Logger* m_instance;
	const std::shared_ptr<spdlog::logger> m_logger;
};