#pragma once

#include <QMessageLogger>
#include <QUdpSocket>

#include <string>
#include <thread>

class JTFF_DTC_API_EXPORT BaseConnector
{
public:
	BaseConnector(const std::string& host, const unsigned int& port);
	~BaseConnector();

protected:
	void sendKey(const std::string& key, const bool& raw = false, const int& delayBefore = 0.25, const int& delayAfter = 0.25);

private:
	const std::shared_ptr<spdlog::logger> m_logger;

	QUdpSocket* m_socket;
	const QHostAddress* m_host;
	const int m_port;
};
