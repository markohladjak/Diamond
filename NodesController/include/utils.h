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

//#include <random>
//#include <sstream>
//
//namespace uuid {
//    static std::random_device              rd;
//    static std::mt19937                    gen(rd());
//    static std::uniform_int_distribution<> dis(0, 15);
//    static std::uniform_int_distribution<> dis2(8, 11);
//
//    std::string generate_uuid_v4() {
//        std::stringstream ss;
//        int i;
//        ss << std::hex;
//        for (i = 0; i < 8; i++) {
//            ss << dis(gen);
//        }
//        ss << "-";
//        for (i = 0; i < 4; i++) {
//            ss << dis(gen);
//        }
//        ss << "-4";
//        for (i = 0; i < 3; i++) {
//            ss << dis(gen);
//        }
//        ss << "-";
//        ss << dis2(gen);
//        for (i = 0; i < 3; i++) {
//            ss << dis(gen);
//        }
//        ss << "-";
//        for (i = 0; i < 12; i++) {
//            ss << dis(gen);
//        };
//        return ss.str();
//    }
//}

#endif /* UTILS_H_ */
