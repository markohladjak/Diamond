/*
 * DeviceEvent.h
 *
 *  Created on: Apr 20, 2021
 *      Author: rem
 */

#ifndef NET_COMDATA_DEVICEEVENT_H_
#define NET_COMDATA_DEVICEEVENT_H_

#include <Net/ComData/ComData.h>

namespace diamon {

class DeviceEvent: public CommunicationData
{
protected:
	void serialize(JsonDocument &json) const override { }
	bool parse(JsonDocument &json) override { return true; }

public:
//	String getType() const override { return "DeviceEvent"; }
	virtual String deviceType() const = 0;
	String dataType() const override { return "Event"; };

};

} /* namespace diamon */

#endif /* NET_COMDATA_DEVICEEVENT_H_ */
