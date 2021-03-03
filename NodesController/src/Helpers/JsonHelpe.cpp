/*
 * JsonHelpe.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: rem
 */

#include <Helpers/JsonHelper.h>

namespace diamon {

JsonHelper::JsonHelper(const String& jsonString)
{
	deserializeJson(*this, jsonString);
}

} /* namespace diamon */
