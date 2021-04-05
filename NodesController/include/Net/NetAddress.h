/*
 * NetAddress.h
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_NETADDRESS_H_
#define INCLUDE_NET_NETADDRESS_H_

#include <utils.h>

namespace diamon {

typedef uint64_t TNETID;

class NetAddress {
	TNETID ID;

public:
	static const TNETID BROADCAST = 0xFFFFFFFFFFFFFFFF;
	static const TNETID SERVER = 0x0;

	NetAddress(): ID(0) { }
	NetAddress(TNETID id) { ID = id; }

	operator TNETID () { return ID; }
	TNETID operator = (TNETID id) { return ID = id; }
	bool operator == (TNETID id) const { return ID == id; }
	bool operator != (TNETID id) const { return ID != id; }

	bool operator<(const NetAddress &addr) const { return ID < addr.ID; }

	operator String () { return utils::UINT64ToString(ID); }

	String ToString() const { return utils::UINT64ToString(ID); }

	static inline NetAddress FromString(String val) {
		if (val.toLowerCase(), val == "broadcast")
			return NetAddress::BROADCAST;

		return NetAddress(utils::StringToUINT64(val));
	}

	static NetAddress Generate(byte id) { return utils::GenerateDeviceID(id); }
};

}

#endif /* INCLUDE_NET_NETADDRESS_H_ */
