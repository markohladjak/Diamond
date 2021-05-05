/*
 * LiftInfo.h
 *
 *  Created on: Apr 29, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_COMDATA_LIFTINFO_H_
#define INCLUDE_NET_COMDATA_LIFTINFO_H_

#include "DeviceInfo.h"
#include "LiftState.h"
#include "LiftEvent.h"

namespace diamon {

class LiftInfo : public DeviceInfo
{
protected:
	void serialize(JsonDocument &json) const override {
		DeviceInfo::serialize(json);

		json[LIFT_MSG_TAG_STATE] = State.ToString();
	}

	bool parse(JsonDocument &json) override {
		if (!DeviceInfo::parse(json))
			return false;

		State = LiftState::FromString(json[LIFT_MSG_TAG_STATE]);

		return true;
	}

public:
	String getType() const override { return "LiftInfo"; }

	LiftState State;
};

}

#endif /* INCLUDE_NET_COMDATA_LIFTINFO_H_ */
