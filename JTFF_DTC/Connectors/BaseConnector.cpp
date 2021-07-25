#include "BaseConnector.h"
#include <iostream>

BaseConnector::BaseConnector(const ConnectorCreator& connector)
	: m_socket(new QUdpSocket(connector.parent)), m_host(new QHostAddress(connector.host.c_str())), m_port(connector.port), m_logger(connector.logger), m_config(connector.config)
{
	m_logger->debug(("Connecting to host " + connector.host + " :" + std::to_string(m_port) + ".").c_str());
	m_socket->connectToHost(*m_host, m_port);
	if (m_socket->state() != QAbstractSocket::ConnectedState)
		m_logger->critical(("Unable to connect to host " + connector.host + " :" + std::to_string(m_port) + ".").c_str());
}

BaseConnector::~BaseConnector()
{
	m_logger->debug(("Disonnecting from host " + m_host->toString().toStdString() + " :" + std::to_string(m_port) + ".").c_str());
	m_socket->disconnectFromHost();
	if (m_socket->state() == QAbstractSocket::ConnectedState)
		m_logger->critical("Unable to disconnect socket from host.");

	delete m_socket, m_host, m_logger;
}

void BaseConnector::sendKey(const std::string& key, const bool raw, const int delayAfter)
{
	short int length;
	if (!raw) {
		length = m_socket->write((key + " 1\n").c_str());
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		m_socket->write((key + " 0\n").c_str());
	}
	else
		length = m_socket->write((key + "\n").c_str());

	std::this_thread::sleep_for(std::chrono::milliseconds(delayAfter));

	if (length != (key.size() + ((raw) ? 1 : 3)))
		m_logger->critical("Key " + key + " can't be send.");
}

Waypoint BaseConnector::parseCoords(const std::string& coords, const bool& IsNavgrid)
{
	// Extract infos from line
	std::string tmp = coords;
	std::vector<std::string> infos;
	size_t pos = 0;
	while ((pos = tmp.find(' ')) != std::string::npos) {
		infos.push_back(tmp.substr(0, pos));
		tmp.erase(0, pos + 1);
	}
	infos.push_back(tmp);

	Waypoint res;
	int i = (IsNavgrid) ? 0 : 1;
	int N = infos[i].find_first_of("N"), E = infos[i].find_first_of("E"), S = infos[i].find_first_of("S"), W = infos[i].find_first_of("W"), A = infos[i].find_first_of("A");
	res.N = (N != std::string::npos) ? infos[i].substr(N + 1, 5) : "";
	res.E = (E != std::string::npos) ? infos[i].substr(E + 1, 5) : "";
	res.S = (S != std::string::npos) ? infos[i].substr(S + 1, 5) : "";
	res.W = (W != std::string::npos) ? infos[i].substr(W + 1, 5) : "";

	if (!IsNavgrid) {
		res.ALT = infos[2];
		res.TOT = infos[3];
		res.NAME = infos[4];
	}
	return res;
}
