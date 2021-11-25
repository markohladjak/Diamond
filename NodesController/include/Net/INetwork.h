/*
 * INetwork.h
 *
 *  Created on: Nov 15, 2021
 *      Author: rem
 */

#ifndef INCLUDE_NET_INETWORK_H_
#define INCLUDE_NET_INETWORK_H_

#include "NetAddress.h"

#include <list>

namespace diamon {

class NetAddressList: public std::list<NetAddress>
{};

typedef void (*receive_funct_t)(uint8_t *data, int lenght);

typedef std::function<void(uint8_t *data, int lenght)> data_receive_callback_t;

class INetwork
{
public:
	virtual int tttsend(uint8_t *data, int lenght) = 0;

	virtual void receive_callback_register(data_receive_callback_t callback_funct) = 0;

	virtual NetAddressList devices_address_list() = 0;
};

} /* namespace diamon */

#endif /* INCLUDE_NET_INETWORK_H_ */
