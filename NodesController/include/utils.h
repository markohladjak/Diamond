/*
 * utils.h
 *
 *  Created on: Feb 9, 2021
 *      Author: rem
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

namespace diamon {

class utils {
public:
	utils();
	virtual ~utils();

	static String ESPEFuseMacStr();

};

} /* namespace diamon */

#endif /* UTILS_H_ */
