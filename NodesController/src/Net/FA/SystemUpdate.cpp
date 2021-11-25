/*
 * SystemUpdate.cpp
 *
 *  Created on: Nov 15, 2021
 *      Author: rem
 */

#include "Net/FA/SystemUpdate.h"

#include "Update.h"

namespace diamon {

using namespace std;

enum pkt_types
{
	BEGIN,
	UPDATE,
	END
};

struct update_pkt
{
	uint16_t data_len;
	uint8_t type;
	uint8_t data;

	update_pkt(uint8_t _type, uint16_t _len = 0): data_len {_len}, type {_type}, data {0} {}
	uint8_t* data_ptr() { return &data; }

	uint16_t size() { return 3 + (data_len ? data_len : 1); }

	operator uint8_t* () { return (uint8_t*)this; }

	static update_pkt* create(uint8_t _type, uint16_t _len, const uint8_t *data)
	{
//		auto pkt = (update_pkt*) new uint8_t[3 + _len ? _len : 1];
		static auto pkt = (update_pkt*) new uint8_t[2048];


		pkt->type = _type;
		pkt->data_len = _len;

		printf("static update_pkt* create     type: %d    data_len: %d     _len: %d\n", pkt->type, pkt->data_len, _len);

		memcpy(&(pkt->data), data, _len);

		printf("....\n");

		return pkt;
	}
private:
};

SystemUpdate::SystemUpdate(INetwork *network)
		: _network { network }
{
	using namespace std::placeholders;

	_receive_callback_ptr = bind(&SystemUpdate::on_receive, this, _1, _2);

	if (network)
	{
		network->receive_callback_register(_receive_callback_ptr);
	}
}

void SystemUpdate::on_receive(uint8_t *data, int lenght)
{
	printf("SystemUpdate::on_receive\n");
	update_pkt &pkt = *((update_pkt*)data);

	printf("type: %d    data_len: %d     obj_size: %d\n", pkt.type, pkt.data_len, pkt.size());

	switch(pkt.type)
	{
		case BEGIN:
			Update.begin();
			break;
		case UPDATE:
			Update.write(pkt.data_ptr(), pkt.data_len);
			break;
		case END:
			if (Update.end());
				esp_restart();
			break;
	}
}

void SystemUpdate::begin() {
	update_pkt pkt(BEGIN);

	_network->tttsend(pkt, pkt.size());
}

void SystemUpdate::update(uint8_t *data, int lenght) {
	printf("S1\n");

	auto pkt = update_pkt::create(UPDATE, lenght, data);
	printf("S2\n");

	_network->tttsend(*pkt, pkt->size());
	printf("S3\n");

//	auto pkt = (update_pkt*) new uint8_t[3 + _len ? _len : 1];

	//	delete []((uint8_t*)pkt);b
	printf("S4\n");

}

void SystemUpdate::end()
{
	update_pkt pkt(END);

	_network->tttsend(pkt, pkt.size());
}

} /* namespace diamon */
