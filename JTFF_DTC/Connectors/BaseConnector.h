#ifndef BASECONNECTOR_H
#define BASECONNECTOR_H

#include <QMessageLogger>
#include <QUdpSocket>

#include <string>
#include <thread>

#include "../DTCLogger.h"
#include "../DTCSettings.h"

struct ConnectorCreator {
	QObject* parent;
	const std::string host;
	const int port;
	DTCLogger* logger;
	DTCSettings* config;
};

struct Waypoint {
	std::string N, E, S, W, ALT, NAME;
};

class BaseConnector
{
public:
	BaseConnector(const ConnectorCreator& connector);
	~BaseConnector();

protected:
	void sendKey(const std::string& key, const bool raw = false, const int delayAfter = 0.25);
	Waypoint parseCoords(const std::string& coords, const bool& IsNavgrid = false);
	DTCLogger* getLogger() { return m_logger; };
	
private:
	QUdpSocket* m_socket;
	const QHostAddress* m_host;
	const int m_port;

	DTCLogger* m_logger;
	DTCSettings* m_config;
};

#endif //!BASECONNECTOR_H
