/*
 * utils.h
 *
 *  Created on: Feb 9, 2021
 *      Author: rem
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>
#include <functional>
#include <typecase.h>

namespace diamon {

class utils {
public:
	utils();
	virtual ~utils();

	static String ESPEFuseMacStr();
	static uint64_t GenerateDeviceID(byte id);
	static String UINT64ToString(uint64_t val);
	static uint64_t StringToUINT64(String str);
	static String GetVersion();
};

template <typename T, typename B>
void action_if(B value, std::function<void(T)> action)
{
    auto cast_value = dynamic_cast<T*>(value);
    if (cast_value != nullptr)
    {
        action(cast_value);
    }

//    __cplusplus
}

} /* namespace diamon */

#endif /* UTILS_H_ */
