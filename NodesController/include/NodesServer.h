/*
 * NodesServer.h
 *
 *  Created on: Feb 19, 2021
 *      Author: rem
 */

#ifndef NODESSERVER_H_
#define NODESSERVER_H_

#include <Arduino.h>
#include <map>
#include <LiftState.h>
#include <Events.h>
#include <Net/INetService.h>

namespace diamon {

class Device {
public:
	NetAddress _address;
	LiftState _state;
	NCVersion _version;
};

typedef std::map<NetAddress, Device> DeviceList;

class NodesServer {
	DeviceList _devices;

	INetService *_netService;

	void set_status(NetAddress addr, LiftState state, NCVersion ver);
	void request_report_all();

public:
	NodesServer(INetService *netService = NULL);
	virtual ~NodesServer();

	size_t DevicesCount();
	DeviceList& DevicesList();

	void RequestState(NetAddress id, LiftState state);
	void ReportAll();
	void ResetAll(LiftState state = LiftState::NONE);

	TEvent<NetAddress, LiftState> DeviceAddedEvent;
	TEvent<NetAddress, LiftState> StateChangedEvent;


	void OnNetMessage(NetAddress addr, NetMessage *msg);
	void OnNetworkChanged();
};

} /* namespace diamon */

#endif /* NODESSERVER_H_ */
