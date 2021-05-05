/*
 * DeviceNameChangedEvent.h
 *
 *  Created on: May 1, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_COMDATA_DDEVICENAMECHANGEDEVENT_H_
#define INCLUDE_NET_COMDATA_DDEVICENAMECHANGEDEVENT_H_

#include <Net/ComData/ComTypes.h>

namespace diamon {

#define DEVICE_MSG_TAG_NEWNAME "newname"
#define DEVICE_MSG_TAG_OLDNAME "oldname"

class DDeviceNameChangedEvent: public DeviceEvent
{
protected:
	bool parse(JsonDocument &json) override {
		if (!DeviceEvent::parse(json))
			return false;

		NewName = json[DEVICE_MSG_TAG_NEWNAME].as<String>();
		OldName = json[DEVICE_MSG_TAG_OLDNAME].as<String>();

		return true;
	}

	void serialize(JsonDocument &json) const override {
		DeviceEvent::serialize(json);

		json[DEVICE_MSG_TAG_NEWNAME] = NewName;
		json[DEVICE_MSG_TAG_OLDNAME] = OldName;
	}

public:
	String getType() const override { return "DeviceNameChangedEvent"; }
	String deviceType() const override { return "Device"; }

	String NewName;
	String OldName;
};

} /* namespace diamon */

#endif /* INCLUDE_NET_COMDATA_DDEVICENAMECHANGEDEVENT_H_ */
