/*
 * LiftEvent.h
 *
 *  Created on: Apr 20, 2021
 *      Author: rem
 */

#ifndef NET_COMDATA_LIFTEVENT_H_
#define NET_COMDATA_LIFTEVENT_H_

#include <Net/ComData/DeviceEvent.h>

namespace diamon {

#define LIFT_MSG_TAG_STATE "state"

class LiftEvent: public DeviceEvent
{
protected:
	void serialize(JsonDocument &json) const override { }
	bool parse(JsonDocument &json) override { return true; }

public:
//	String getType() const override { return "LiftEvent"; }
	String deviceType() const override { return "Lift"; }

};

} /* namespace diamon */

#endif /* NET_COMDATA_LIFTEVENT_H_ */
