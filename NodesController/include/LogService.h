/*
 * LogService.h
 *
 *  Created on: Jan 26, 2021
 *      Author: rem
 */

#ifndef LOGSERVICE_H_
#define LOGSERVICE_H_

#include <Arduino.h>

namespace diamon {

#define Log__(x) LogService::Log(x, "")

class LogService {
	LogService *_instance;

public:
	LogService();
	static void Log(String source, String msg);
	static void Println(String msg);
};

} /* namespace diamon */

#endif /* LOGSERVICE_H_ */
