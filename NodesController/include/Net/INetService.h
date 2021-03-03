/*
 * NetService.h
 *
 *  Created on: Jan 27, 2021
 *      Author: rem
 */

#ifndef NETSERVICE_H_
#define NETSERVICE_H_

#include <IInLoop.h>
#include <Events.h>
#include <Net/NetMessage.h>

namespace diamon {

class INetService {
public:
	virtual ~INetService() { };

	virtual std::list<NetAddress> GetConnectedDevices() = 0;

	virtual void Send(NetMessage& msg, NetAddress to = NetAddress::BROADCAST) = 0;

	virtual void OnReceive(NetAddress from, void (*onReceive)(NetMessage &msg)) = 0;

	TEvent<NetAddress, NetMessage*> OnReceiveEvent;
};

} /* namespace diamon */

#endif /* NETSERVICE_H_ */
