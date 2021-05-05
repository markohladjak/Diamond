/*
 * DeviceInfo.h
 *
 *  Created on: Apr 29, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_COMDATA_DEVICEINFO_H_
#define INCLUDE_NET_COMDATA_DEVICEINFO_H_

#include "ComData.h"

namespace diamon {

#define MSG_TAG_DEVICE_INFO_NAME "name"

class DeviceInfo : public CommunicationData
{
protected:
	bool parse(JsonDocument &json) override {
		Name = json[MSG_TAG_DEVICE_INFO_NAME].as<String>();

		return true;
	}

	void serialize(JsonDocument &json) const override {
		json[MSG_TAG_DEVICE_INFO_NAME] = Name;
	}

public:
	String getType() const override { return "DeviceInfo"; }
	String dataType() const override { return "Info"; };

	String Name;
};

}

#endif /* INCLUDE_NET_COMDATA_DEVICEINFO_H_ */
