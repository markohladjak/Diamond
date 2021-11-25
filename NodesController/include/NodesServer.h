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
#include <Net/NCVersion.h>

#include <Net/FA/SystemUpdate.h>

namespace diamon {

class Device {
public:
	NetAddress _address;
	LiftState _state;
	NCVersion _version;
	String _name;
};

typedef std::map<NetAddress, Device> DeviceList;

class NodesServer {
	DeviceList _devices;

	INetService *_netService;

	SystemUpdate *_update;

	void set_info(NetAddress addr, LiftState state, String name);
	void set_status(NetAddress addr, LiftState state);
	void set_name(NetAddress addr, String name);
	void request_report_all();

public:
	NodesServer(INetService *netService = NULL, SystemUpdate *update = NULL);
	virtual ~NodesServer();

	size_t DevicesCount();
	DeviceList& DevicesList();

	void RequestState(NetAddress id, LiftState state);
	void ChangeDeviceName(NetAddress id, String name);

	void ReportAll();
	void ResetAll(LiftState state = LiftState::NONE);

	TEvent<NetAddress, LiftState, String> DeviceAddedEvent;
	TEvent<NetAddress, LiftState> StateChangedEvent;
	TEvent<NetAddress, String> DeviceNameChangedEvent;


	void OnNetMessage(NetAddress addr, NetMessage *msg);
	void OnNetworkChanged();

	void UpdateBegin(int partition = 1);
	void UpdateFA(size_t index, uint8_t *data, size_t len);
	void UpdateCommit(bool reboot = true);
};

} /* namespace diamon */

#endif /* NODESSERVER_H_ */
