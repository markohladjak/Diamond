/*
 * JsonHelpe.h
 *
 *  Created on: Feb 27, 2021
 *      Author: rem
 */

#ifndef HELPERS_JSONHELPE_H_
#define HELPERS_JSONHELPE_H_

#include <ArduinoJson.h>

namespace diamon {

class JsonHelper: public StaticJsonDocument<0x100>
{
public:
	JsonHelper() {}
	JsonHelper(const String& jsonString);

	operator String () const;
};


} /* namespace diamon */

#endif /* HELPERS_JSONHELPE_H_ */
