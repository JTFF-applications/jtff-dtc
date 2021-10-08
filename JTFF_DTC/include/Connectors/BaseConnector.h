#ifndef BASECONNECTOR_H
#define BASECONNECTOR_H

#include <QMessageLogger>
#include <QUdpSocket>

#include <string>
#include <thread>

#include "../DTCLogger.h"
#include "../DTCSettings.h"
#include "../DTCUtils.h"

struct JTFF_DTC_API ConnectorCreator {
	QObject* parent;
	const std::string host;
	const int port;
	DTCLogger* logger;
	DTCSettings* config;
};

struct JTFF_DTC_API Waypoint {
	std::string N, E, S, W, ALT, NAME;
};

class JTFF_DTC_API BaseConnector
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
