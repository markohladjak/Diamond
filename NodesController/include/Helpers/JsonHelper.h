/*
 * JsonHelpe.h
 *
 *  Created on: Feb 27, 2021
 *      Author: rem
 */

#ifndef HELPERS_JSONHELPE_H_
#define HELPERS_JSONHELPE_H_

#include <ArduinoJson.h>

#define JSON_STRING_MAX_LENGHT 0x100

namespace diamon {

//class JsonHelper: public StaticJsonDocument<JSON_STRING_MAX_LENGHT>
//{
//public:
//	JsonHelper() {}
//	JsonHelper(const String& jsonString);
//
//	operator String () const;
//};

class JsonHelper: public DynamicJsonDocument
{
public:
	JsonHelper() : DynamicJsonDocument(JSON_STRING_MAX_LENGHT) {}
	JsonHelper(const String& jsonString);

	operator String () const;
};


} /* namespace diamon */

#endif /* HELPERS_JSONHELPE_H_ */
