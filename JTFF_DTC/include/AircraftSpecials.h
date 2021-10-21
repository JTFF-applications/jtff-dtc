#pragma once

#include <string>

namespace Hornet
{
	static int MaxWaypoints = 100;
}

namespace Tomcat
{
	enum class UsableWaypoints
	{
		WP1 = 1, WP2, WP3, FP, IP, HB, DP, HA, ST, Error
	};

	struct NavgridParams
	{
		std::string center;
		int sectors, width, orientation;
	};

	UsableWaypoints StringToWP(const std::string& wp);
	std::string WPToSring(const UsableWaypoints& wp);
}
