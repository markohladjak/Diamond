/*
 * LiftStateChangedEvent.h
 *
 *  Created on: Apr 20, 2021
 *      Author: rem
 */

#ifndef NET_COMDATA_LIFTSTATECHANGEDEVENT_H_
#define NET_COMDATA_LIFTSTATECHANGEDEVENT_H_

#include <Net/ComData/LiftEvent.h>
#include <LiftState.h>

namespace diamon {


class LiftStateChangedEvent: public LiftEvent
{
protected:
	void serialize(JsonDocument &json) const override;
	bool parse(JsonDocument &json) override;

public:
	LiftState State;

	String getType() const override { return "LiftStateChangedEvent"; }
};

} /* namespace diamon */

#endif /* NET_COMDATA_LIFTSTATECHANGEDEVENT_H_ */
