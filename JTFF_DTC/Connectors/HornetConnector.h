#ifndef HORNETCONNECTOR_H
#define HORNETCONNECTOR_H

#include "BaseConnector.h"

class HornetConnector : public BaseConnector
{
public:
	HornetConnector(const ConnectorCreator& base);
	~HornetConnector();

	static int GetMaxWaypoints() { return 100; }
private:

};

#endif //!HORNETCONNECTOR_H