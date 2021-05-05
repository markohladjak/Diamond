/*
 * DeviceCommand.h
 *
 *  Created on: Apr 20, 2021
 *      Author: rem
 */

#ifndef NET_COMDATA_DEVICECOMMAND_H_
#define NET_COMDATA_DEVICECOMMAND_H_

#include "ComData.h"

namespace diamon {

class DeviceCommand: public CommunicationData
{
protected:
	void serialize(JsonDocument &json) const override { }
	bool parse(JsonDocument &json) override { return true; }

public:
	String getType() const override { return "DeviceCommand"; }
	String dataType() const override { return "Command"; };

};

class DeviceGetInfoCommand : public DeviceCommand
{
protected:
//	virtual void serialize(JsonDocument *json) const override { }
//	bool parse(JsonDocument &json) override { return true; }

public:
	String getType() const override { return "DeviceGetInfoCommand"; }

};

} /* namespace diamon */

#endif /* NET_COMDATA_DEVICECOMMAND_H_ */
