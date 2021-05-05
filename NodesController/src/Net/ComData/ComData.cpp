/*
 * ComData.cpp
 *
 *  Created on: Apr 20, 2021
 *      Author: rem
 */

#include <Net/ComData/ComData.h>
#include <Helpers/JsonHelper.h>

#include <LogService.h>

namespace diamon {

CommunicationData::~CommunicationData() {
}

bool CommunicationData::FromJson(const String &jsonString) {
	JsonHelper json(jsonString);

	parse(json);

	return true;
}

String CommunicationData::ToJson() {
	JsonHelper json;

	serialize(json);

	return json;
}

} /* namespace diamon */
