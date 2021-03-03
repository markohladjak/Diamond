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
	friend class NetService;

protected:
	NetAddress _from;
	NetAddress _to;

public:
	NetEvent Event;
	DeviceType Type;

	NetMessage() {}
	NetMessage(const String& json) { Deserialize(json); }

	virtual ~NetMessage() {}

	virtual operator String ();

	virtual NetMessage& operator=(const String& json)
	{
		Deserialize(json);
		return *this;
	}

	virtual void Deserialize(const String& json);
};

class LiftNetMessage: public NetMessage
{
	void Deserialize(const String& json) override;

public:
	LiftNetMessage() {
		Type = DeviceType::LIFT;
	}

	LiftNetMessage(const String& json) { Deserialize(json); }

	LiftState State;

	operator String () override;



};

}

#endif /* INCLUDE_NETMSG_H_ */
