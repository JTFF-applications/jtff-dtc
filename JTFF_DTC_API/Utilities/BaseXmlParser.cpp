#include <QMessageBox>
#include <QFileDialog>

#include <filesystem>
#include <format>
#include <fstream>
#include <pugixml.hpp>

#include "BaseXmlParser.h"

BaseXmlParser::BaseXmlParser(const std::string& aircraft, const std::string& filePath)
	: m_aircraft(aircraft), m_file(filePath), m_logger(Logger::Get())
{
	if (m_file.empty())
	{
		std::string filePath = QFileDialog::getSaveFileName(nullptr, "Export Waypoints", "", "XML File (*.xml)").toStdString();
		if (filePath.empty())
			return;

		m_file = filePath;

		pugi::xml_document doc;
		pugi::xml_node declaration = doc.append_child(pugi::node_declaration);
		declaration.append_attribute("version") = "1.0";
		declaration.append_attribute("encoding") = "ISO-8859-1";
		declaration.append_attribute("standalone") = "yes";

		pugi::xml_node root = doc.append_child("Aircraft");
		root.append_attribute("name") = m_aircraft.c_str();

		SaveDocument(doc);
		return;
	}

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(m_file.c_str(), pugi::parse_default | pugi::parse_declaration);
	if (!result)
		throw bad_parser(ERROR_TYPE::BAD_FILE, m_file.c_str());

	if (std::string(doc.document_element().attribute("name").value()) != m_aircraft)
		throw bad_parser(ERROR_TYPE::BAD_AIRCRAFT, m_file.c_str());
}

BaseXmlParser::~BaseXmlParser()
{
}

void BaseXmlParser::SaveDocument(const pugi::xml_document& doc)
{
	if (!doc.save_file(m_file.c_str(), PUGIXML_TEXT("  ")))
	{
		QMessageBox(QMessageBox::Icon::Warning, "XML Error", "Failed to save xml file. Check folder write access.", QMessageBox::Ok).exec();
		m_logger->error(std::format("Failed to save xml file : {} !", m_file));
		throw bad_parser(ERROR_TYPE::BAD_FILE, m_file.c_str());
	}
}
