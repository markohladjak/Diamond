/*
 * NetMsg.h
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NETMSG_H_
#define INCLUDE_NETMSG_H_

#include "../LiftState.h"
#include "NetAddress.h"
#include <IDevice.h>
#include <Net/NetEvent.h>

namespace diamon {

class NetMessage
{
	friend class ESP32MeshNetService;

protected:
	NetAddress _from;
	NetAddress _to;

public:
	NetEvent Event;
	DeviceType Type;

	NetMessage() {}
	NetMessage(const String& json) { Deserialize(json); }

	virtual ~NetMessage() {}

	DeviceType GetDeviceType() { return Type; }

	virtual operator String ();

	virtual NetMessage& operator=(const String& json)
	{
		Deserialize(json);
		return *this;
	}

	virtual void Deserialize(const String& json);

	static NetMessage* Resolve(const String &jsonString);
};

class LiftNetMessage: public NetMessage
{
	void Deserialize(const String& json) override;

public:
	LiftState State;

	LiftNetMessage() {
		Type = DeviceType::LIFT;
	}

	LiftNetMessage(const String& json) { Deserialize(json); }


	operator String () override;
};

}

#endif /* INCLUDE_NETMSG_H_ */
