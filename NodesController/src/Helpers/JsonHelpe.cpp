/*
 * JsonHelpe.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: rem
 */
#include <Helpers/JsonHelper.h>

namespace diamon {

JsonHelper::JsonHelper(const String& jsonString)
		: DynamicJsonDocument(JSON_STRING_MAX_LENGHT)
{
	assert(capacity() >= jsonString.length());

	deserializeJson(*this, jsonString);
}

JsonHelper::operator String() const {
	String json_str;
	serializeJson(*this, json_str);

	return json_str;
}

} /* namespace diamon */
