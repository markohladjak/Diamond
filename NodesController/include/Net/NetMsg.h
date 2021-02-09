/*
 * NetMsg.h
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NETMSG_H_
#define INCLUDE_NETMSG_H_

#include "../LiftState.h"
#include "NetAddress.h"

namespace diamon {


//class DeviceType {
//public:
//	enum {
//		NONE,
//		POST,
//		DOOR
//	};
//
//	DeviceType(): _type(NONE) { }
//
//	operator int () { return _type; }
//	int64_t operator = (int64_t type) { return _type = type; }
//	bool operator == (int64_t type) { return _type == type; }
//
//private:
//	int _type = NONE;
//};


class NetMsg
{
public:
	 String msg;
};

}


#endif /* INCLUDE_NETMSG_H_ */
