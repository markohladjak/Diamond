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

#include <WString.h>

namespace diamon {

class NCVersion {
public:

	uint8_t major = 0;
	uint8_t minor = 0;
	uint8_t build = 0;
	uint16_t revision = 0;

	NCVersion() {}

	static NCVersion FromString(String version)
	{
		uint16_t v[4];
		int vp = 0;
		String s;

		for (int i = 0; i < version.length(); ++i)
		{
			if (version.c_str()[i] != '.')
				s += version.c_str()[i];
			else {
				v[vp++] = s.toInt();
				s = "";
			}
		}

		v[vp++] = s.toInt();
		s = "";

		NCVersion ver;

		ver.major = v[0];
		ver.minor = v[1];
		ver.build = v[2];
		ver.revision = v[3];

		return ver;
	}

	String ToString() { return String(major) + "." + String(minor) + "." + String(build) + "." + String(revision); }
};

class NetMessage
{
	friend class ESP32MeshNetService;

protected:
	NetAddress _from;
	NetAddress _to;

public:
	NetEvent Event;
	DeviceType Type;

	NCVersion Version;

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
