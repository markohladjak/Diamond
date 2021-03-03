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

	static bool IsRoot;

	NetService(NetAddress address);
	~NetService() override;

	void Send(NetMessage& msg, NetAddress to = NetAddress::BROADCAST) override;

	std::list<NetAddress> GetConnectedDevices() override;

	void OnReceive(diamon::NetAddress from,	void (*onReceive)(diamon::NetMessage&)) override;

	static void update();

private:
	static painlessMesh *mesh;

	static std::map<NetAddress, NetService*> localNodes;

	static void initMesh();

	static void receivedCallback(uint32_t from, TSTRING &msg);

	static void gotIPCallback(WiFiEvent_t event, WiFiEventInfo_t info);

};

} /* namespace diamon */

#endif /* NET_NETSERVICE_H_ */
