#ifndef DTCUTILS_H
#define DTCUTILS_H

#include <string>
#include <sstream>

#include "pugixml/pugixml.hpp"

#include "DTCLogger.h"
#include "DTCSettings.h"

#define JTFF_DTC_API __declspec(dllexport)

namespace DTCUtils
{    
    JTFF_DTC_API std::string decimalToLatLong(const std::string& coords, const unsigned int& precision, bool isLat);
    JTFF_DTC_API void OpenErrorBox(const std::string& msg);
    JTFF_DTC_API void ReplaceOccurencesInStr(std::string& string, const char& occur, const std::string& toReplace);

    enum class JTFF_DTC_API Aircraft
    {
        Tomcat, Hornet, Viper, Warthog
    };
}

#endif // !DTCUTILS_H