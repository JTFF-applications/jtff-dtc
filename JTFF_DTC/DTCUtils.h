#ifndef DTCUTILS_H
#define DTCUTILS_H

#include <string>
#include <sstream>

#include "pugixml/pugixml.hpp"

#include "DTCLogger.h"
#include "DTCSettings.h"
#include "Windows/WaypointAddDialog.h"
#include "Windows/FileSelectorWindow.h"

namespace DTCUtils
{    
    std::string decimalToLatLong(const std::string& coords, const unsigned int& precision, bool isLat);
}

#endif // !DTCUTILS_H