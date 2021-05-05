/*
 * NetMsg.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: rem
 */
#include <Net/NetMessage.h>
#include <Helpers/JsonHelper.h>
#include <Net/ComData/ComTypes.h>


namespace diamon {

#define MSG_TAG_FROM "from"
#define MSG_TAG_TO "to"
#define MSG_TAG_TYPE "type"
#define MSG_TAG_CONTEXT "ctx"


void NetMessage::Deserialize(const String& jsonString)
{
	const JsonHelper json(jsonString);

	_from = NetAddress::FromString(json[MSG_TAG_FROM]);
	_to = NetAddress::FromString(json[MSG_TAG_TO]);

	auto data = ComTypes::resolve(json[MSG_TAG_TYPE], json[MSG_TAG_CONTEXT]);

	if (data)
		_context = data;
}

NetMessage::operator String() {
	JsonHelper json;

	json[MSG_TAG_FROM] = _from.ToString();
	json[MSG_TAG_TO] = _to.ToString();
	json[MSG_TAG_TYPE] = Context().getType();

	json[MSG_TAG_CONTEXT] = Context().ToJson();

	return json;
}

void NetMessage::SetContext(CommunicationData *ctx) {
	_context.reset(ctx);
}

}

