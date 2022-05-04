#pragma once

#include <QMessageLogger>
#include <QUdpSocket>

#include <string>
#include <thread>

#include "../Utilities/Waypoint.h"
#include "../Logger.h"

class JTFF_DTC_API_EXPORT BaseConnector
{
public:
	BaseConnector(const std::string& host, const unsigned int& port);
	~BaseConnector();

	virtual void EnterWaypoints(std::list<std::pair<const std::string, const Waypoint>> waypoints) = 0;
	virtual void EnterWaypoint(std::pair<const std::string, const Waypoint> waypoint) = 0;

protected:
	void SendKey(const std::string& key, const bool& raw = false, const int& delayBefore = 5, const int& delayAfter = 5);
	const std::shared_ptr<spdlog::logger> m_logger;

private:
	QUdpSocket* m_socket;
	const QHostAddress* m_host;
	const int m_port;
};
