/*
 * NetService.h
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#ifndef NET_NETSERVICE_H_
#define NET_NETSERVICE_H_

#include "INetService.h"
#include <painlessMesh.h>
#include <map>
#include <list>

namespace diamon {

class NetService: public INetService {
public:
	NetAddress Address;

	NetService(NetAddress address);
	~NetService() override;

	void Send(diamon::NetAddress to, const diamon::NetMsg &msg) override;
	void SendAll(const diamon::NetMsg &msg) override;

	std::list<NetAddress> GetConnectedDevices() override;

	void OnReceive(diamon::NetAddress from,	void (*onReceive)(diamon::NetMsg&)) override;

	void update() override;

private:
public: static painlessMesh *mesh;

	static std::map<uint64_t, NetService*> nodes;
};

} /* namespace diamon */

#endif /* NET_NETSERVICE_H_ */
