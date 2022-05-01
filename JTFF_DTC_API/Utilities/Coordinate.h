#pragma once

#include <stdexcept>
#include <string>
#include <regex>

#include "../Logger.h"

class JTFF_DTC_API_EXPORT Coordinate
{
public:
	class JTFF_DTC_API_EXPORT bad_coordinate : public std::exception
	{
	public:
		bad_coordinate(const std::string& text)
			:m_text(text)
		{
		}

		const char* what() const throw () { return m_text.c_str(); }

	private:
		const std::string m_text = "";
	};

	enum class Type
	{
		Lattitude, Longitude
	};

public:
	Coordinate() = default;
	~Coordinate() = default;

	void operator=(const Coordinate& other);
	const std::string& ToString() const { return m_coord; }

public:
	static Coordinate FromString(const std::string& coord);
	static Coordinate FromDecimal(const std::string& coord, const Type& type);
	static bool Check(const std::string coord);

private:
	Coordinate(const std::string& coord);

private:
	std::string m_coord = "";
	static const std::regex m_firstValidationRegex;
	static const std::regex m_secondValidationRegex;
};
