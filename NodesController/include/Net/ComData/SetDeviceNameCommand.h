/*
 * SetDeviceNameCommand.h
 *
 *  Created on: May 5, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_COMDATA_SETDEVICENAMECOMMAND_H_
#define INCLUDE_NET_COMDATA_SETDEVICENAMECOMMAND_H_

#include "DeviceCommand.h"

namespace diamon {

#define DEVICE_MSG_TAG_NAME "name"

class SetDeviceNameCommand : public DeviceCommand
{
protected:
	void serialize(JsonDocument &json) const override {
		DeviceCommand::serialize(json);

		json[DEVICE_MSG_TAG_NAME] = Name;
	}

	bool parse(JsonDocument &json) override {
		if (!DeviceCommand::parse(json))
			return false;

		Name = json[DEVICE_MSG_TAG_NAME].as<String>();

		return true;
	}
public:
	String getType() const override { return "SetDeviceNameCommand"; }

	String Name;
};

} /* namespace diamon */

#endif /* INCLUDE_NET_COMDATA_SETDEVICENAMECOMMAND_H_ */
