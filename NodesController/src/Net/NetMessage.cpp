/*
 * NetMsg.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: rem
 */
#include <Net/NetMessage.h>
#include <Helpers/JsonHelper.h>

namespace diamon {


void NetMessage::Deserialize(const String& jsonString)
{
	const JsonHelper json(jsonString);

	_from = NetAddress::FromString(json["from"]);
	_to = NetAddress::FromString(json["to"]);
	Event = json["event"].as<int>();
	Type = json["type"].as<int>();
}

NetMessage::operator String() {
	JsonHelper json;

	json["from"] = _from.ToString();
	json["to"] = _to.ToString();
	json["event"] = (String)Event;
	json["type"] = Type.ToString();

	String str;
	serializeJson(json, str);

	return str;
}

LiftNetMessage::operator String() {
//	auto str = NetMessage::operator String();
	JsonHelper json;

	json["from"] = _from.ToString();
	json["to"] = _to.ToString();
	json["event"] = (String)Event;
	json["type"] = Type.ToString();
	json["state"] = String(State.State);

	String str;
	serializeJson(json, str);

	return str;
}

void LiftNetMessage::Deserialize(const String& jsonString)
{
	NetMessage::Deserialize(jsonString);

	const JsonHelper json(jsonString);

	State = json["state"].as<int>();
}

NetMessage* NetMessage::Resolve(const String &jsonString) {
	NetMessage msg(jsonString);

	switch (msg.Type) {
	case DeviceType::LIFT:
		return new LiftNetMessage(jsonString);
	case DeviceType::DOOR:
	case DeviceType::UNKNOW:
		;
	}

	return NULL;
}

}

