#include "Logger.h"

Logger* Logger::m_instance = new Logger();

Logger::Logger()
	: m_logger(spdlog::basic_logger_mt("logger", "logs/dtc.log"))
{
	m_logger->info("----- New log opened ! -----");
	m_logger->enable_backtrace(32);
}

std::shared_ptr<spdlog::logger> Logger::Get()
{
	return m_instance->m_logger;
}
