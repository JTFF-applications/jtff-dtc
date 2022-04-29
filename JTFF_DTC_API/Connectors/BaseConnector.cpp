#include <iostream>

#include "../Logger.h"
#include "BaseConnector.h"

BaseConnector::BaseConnector(const std::string& host, const unsigned int& port)
	: m_socket(new QUdpSocket()), m_host(new QHostAddress(host.c_str())), m_port(port), m_logger(Logger::Get())
{
	m_socket->connectToHost(*m_host, m_port);
	if (m_socket->state() != QAbstractSocket::ConnectedState)
		m_logger->critical("Unable to connect to host {}:{}.", host, m_port);
	else
		m_logger->info("Connected to {}:{}.", host, m_port);
}

BaseConnector::~BaseConnector()
{
	m_socket->disconnectFromHost();
	if (m_socket->state() == QAbstractSocket::ConnectedState)
		m_logger->critical("Unable to disconnect from {}:{}.", m_host->toString().toStdString(), m_port);
	else
		m_logger->info("Disconnected from {}:{}", m_host->toString().toStdString(), m_port);
	delete m_socket, m_host;
}

void BaseConnector::sendKey(const std::string& key, const bool& raw, const int& delayBefore, const int& delayAfter)
{
	unsigned int length = 0;
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
