#include <QMessageBox>

#include "DTCUtils.h"

inline std::string DegreesMinutesSeconds(double ang, unsigned int num_dec_places = 2)
{
    bool neg(false);
    if (ang < 0.0)
    {
        neg = true;
        ang = -ang;
    }

    int deg = (int)ang;
    double frac = ang - (double)deg;

    frac *= 60.0;
    int min = (int)frac;
    frac = frac - (double)min;

    double sec = nearbyint(frac * 600000.0);
    sec /= 10000.0;

    if (sec >= 60.0)
    {
        min++;
        sec -= 60.0;
    }

    std::ostringstream oss;
    if (neg)
        oss << "-";

    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss << deg << ".";
    oss.fill('0');
    oss.width(2);
    oss << min << ".";
    if (num_dec_places == 0)
    {
        oss.width(2);
        oss.precision(0);
    }
    else
    {
        oss.width(num_dec_places + 3);
        oss.precision(num_dec_places);
    }
    oss << sec;

    return oss.str();
}

//
// Convert degrees, minutes and seconds to decimal degrees.
// 
inline double DecimalDegrees(const std::string& dms)
{
    std::string tmp(dms);
    bool neg(false);

    if ((tmp[tmp.length() - 1] == 'W') || (tmp[tmp.length() - 1] == 'S') || (tmp[0] == '-'))
        neg = true;

    for (unsigned int i = 0; i < tmp.length(); i++)
        if (!isdigit(tmp[i]) && (tmp[i] != '.'))
            tmp[i] = ' ';

    double deg(0.0), min(0.0), sec(0.0);
    std::istringstream iss(tmp);
    iss >> deg >> min >> sec;
    double ang = deg + ((min + (sec / 60.0)) / 60.0);

    if (neg)
        ang *= -1;

    return ang;
}

std::string DTCUtils::decimalToLatLong(const std::string& coords, const unsigned int& precision, bool isLat)
{
    std::string res = DegreesMinutesSeconds(atof(coords.c_str()), 2);
    if (precision <= 2)
        res = res.substr(0, res.size() - 5 + precision);
    else if (precision > 2) {
        std::string last = res.substr(res.size() - 2, 2);
        res = res.substr(0, res.size() - 3) + ((precision == 3) ? std::string(1, last.front()): last);
    }

    if (isLat) {
        if (res[0] == '-')
            return "S" + res;
        return "N" + res;
    }
    if (res[0] == '-')
        return "W" + res;
    return "E" + res;
}

void DTCUtils::OpenErrorBox(std::string msg)
{
    QMessageBox box;
    box.setText(msg.c_str());
    box.exec();
}
