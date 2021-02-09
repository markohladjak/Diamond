/*
 * NetAddress.h
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_NETADDRESS_H_
#define INCLUDE_NET_NETADDRESS_H_


namespace diamon {


class NetAddress {
	uint64_t ID;

public:
	NetAddress(): ID(0) { }
	NetAddress(uint64_t id) { ID = id; }

	operator uint64_t () { return ID; }
	uint64_t operator = (uint64_t id) { return ID = id; }
};

}


#endif /* INCLUDE_NET_NETADDRESS_H_ */
