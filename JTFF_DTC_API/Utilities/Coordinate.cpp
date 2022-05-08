#include <QMessageBox>

#include "Coordinate.h"

const std::regex Coordinate::m_firstValidationRegex = std::regex("^[NS]([0-8][0-9](\\.\\d\\d)\\.([0-9]{1}|[0-9]{3}))$");
const std::regex Coordinate::m_secondValidationRegex = std::regex("^[EW]((0\\d\\d|1\\d\\d)(\\.\\d\\d)\\.([0-9]{1}|[0-9]{3}))$");

long double GetDecimal(const long double& nb)
{
	std::string str = std::to_string(nb);
	return std::stold("0" + str.substr(str.find_first_of('.')));
}

Coordinate::Coordinate(const std::string& coord)
	: m_coord(coord)
{
}

void Coordinate::operator=(const Coordinate& other)
{
	m_coord = other.m_coord;
}

bool Coordinate::Check(const std::string coord)
{
	if (coord.empty())
		throw bad_coordinate("Coordinate is empty !");

	if (coord.front() == 'N' || coord.front() == 'S')
		return std::regex_match(coord, Coordinate::m_firstValidationRegex);
	return std::regex_match(coord, Coordinate::m_secondValidationRegex);
}

Coordinate Coordinate::FromString(const std::string& coord)
{
	if (!Check(coord))
		throw bad_coordinate(std::format("{} is not a valid coordinate !", coord));
	return Coordinate(coord);
}

Coordinate Coordinate::FromDecimal(const std::string& coord, const Type& type)
{
	bool isPositive = coord.front() != '-';
	long double number = std::stold(coord.substr(!isPositive, coord.size()));
	unsigned int floatingSize = coord.size() - coord.find_last_of('.');
	std::stringstream dms;

	dms << std::right << std::setfill('0') << std::setw((type == Type::Lattitude) ? 2 : 3);
	dms << std::to_string(static_cast<unsigned int>(std::truncl(number))); // Degrees
	dms << '.';
	number = GetDecimal(number) * 60;
	dms << std::right << std::setfill('0') << std::setw(2);
	dms << std::to_string(static_cast<unsigned int>(std::truncl(number))); // Minutes
	dms << '.';
	number = GetDecimal(number) * 60;
	dms << std::left << std::setfill('0') << std::setw(3);
	dms << std::to_string(static_cast<unsigned int>(std::truncl(number))); // Seconds

	return Coordinate(((type == Type::Lattitude) ? isPositive ? "N" : "S" : isPositive ? "E" : "W") + dms.str());
}
