/*
 * LogService.cpp
 *
 *  Created on: Jan 26, 2021
 *      Author: rem
 */

#include "LogService.h"

namespace diamon {

void LogService::Log(String source, String msg) {
	Serial.print(source);
	if (source.length())
		Serial.print(": ");

	Serial.println(msg);
}

LogService::LogService() {
	_instance = this;

	Serial.begin(115200);

}

void LogService::Println(String msg) {
	Serial.println(msg);
}

} /* namespace diamon */
