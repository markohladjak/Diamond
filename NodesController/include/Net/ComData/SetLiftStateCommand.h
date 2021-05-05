/*
 * SetLiftStateCommand.h
 *
 *  Created on: Apr 29, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_COMDATA_SETLIFTSTATECOMMAND_H_
#define INCLUDE_NET_COMDATA_SETLIFTSTATECOMMAND_H_

#include "DeviceCommand.h"
#include "LiftInfo.h"

namespace diamon {

class SetLiftStateCommand : public DeviceCommand
{
protected:
	void serialize(JsonDocument &json) const override {
		DeviceCommand::serialize(json);

		json[LIFT_MSG_TAG_STATE] = State.ToString();
	}

	bool parse(JsonDocument &json) override {
		if (!DeviceCommand::parse(json))
			return false;

		State = LiftState::FromString(json[LIFT_MSG_TAG_STATE]);

		return true;
	}
public:
	String getType() const override { return "SetLiftStateCommand"; }

	LiftState State;
};

} /* namespace diamon */

#endif /* INCLUDE_NET_COMDATA_SETLIFTSTATECOMMAND_H_ */
