/*
 * SystemUpdate.h
 *
 *  Created on: Nov 15, 2021
 *      Author: rem
 */

#ifndef NET_FA_SYSTEMUPDATE_H_
#define NET_FA_SYSTEMUPDATE_H_

#include <inttypes.h>

#include <list>

#include "../INetwork.h"

namespace diamon {


class SystemUpdate {
	INetwork *_network;

	data_receive_callback_t _receive_callback_ptr;
	void on_receive(uint8_t *data, int lenght);

public:
	SystemUpdate(INetwork *network);

	void begin();
	void update(uint8_t *data, int len);
	void end();
};

} /* namespace diamon */

#endif /* NET_FA_SYSTEMUPDATE_H_ */
