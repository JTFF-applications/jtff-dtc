#ifndef DTCUTILS_H
#define DTCUTILS_H

#include <string>
#include <sstream>

#include "pugixml/pugixml.hpp"

#include "DTCLogger.h"
#include "DTCSettings.h"

namespace DTCUtils
{    
    std::string decimalToLatLong(const std::string& coords, const unsigned int& precision, bool isLat);
    void OpenErrorBox(std::string msg);
}

#endif // !DTCUTILS_H