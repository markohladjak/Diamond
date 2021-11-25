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
#include <WiFi.h>
#include <Net/NetAddress.h>
#include <Net/ComData/ComData.h>

namespace diamon {

class NetMessage;

typedef std::list<NetAddress> device_list_t;

class INetService {
public:
	static void SetMessageEnds(NetMessage &msg, const NetAddress *from, const NetAddress *to);

	virtual ~INetService() { };

	virtual device_list_t GetConnectedDevices() = 0;

	virtual void Send(NetMessage& msg, NetAddress to = NetAddress::BROADCAST) = 0;
	virtual void Send(CommunicationData* data, NetAddress to = NetAddress::BROADCAST) = 0;

	virtual void OnReceive(NetAddress from, void (*onReceive)(NetMessage &msg)) = 0;

	virtual void setWIFIMode(String ssid, String pw) = 0;

	virtual void GetAPList() = 0;

	TEvent<NetAddress> OnDeviceConnected;
	TEvent<NetAddress> OnDeviceDisconnected;
	TEvent<NetAddress, NetMessage*> OnReceiveEvent;
	TEvent<> OnConnectedEvent;
	TEvent<> OnLayoutChangedEvent;
	TEvent<> OnRootAddressObtainedEvent;
	TEvent<WiFiEvent_t, WiFiEventInfo_t> OnWiFiEvent;

};

} /* namespace diamon */

#endif /* NETSERVICE_H_ */
