/*
 * NetService.h
 *
 *  Created on: Jan 27, 2021
 *      Author: rem
 */

#ifndef NETSERVICE_H_
#define NETSERVICE_H_

#include <IInLoop.h>
#include "NetMsg.h"
#include <Events.h>

namespace diamon {

class INetService:  public IInLoop {
public:
	virtual ~INetService() { };

	virtual std::list<NetAddress> GetConnectedDevices() = 0;

	virtual void Send(NetAddress to, const NetMsg &msg) = 0;
	virtual void SendAll(const NetMsg &msg) = 0;

	virtual void OnReceive(NetAddress from, void (*onReceive)(NetMsg &msg)) = 0;

	TEvent<NetAddress, NetMsg&> OnReceiveEvent;
};

} /* namespace diamon */

#endif /* NETSERVICE_H_ */
