/*
 * NodesServer.cpp
 *
 *  Created on: Feb 19, 2021
 *      Author: rem
 */

#include <NodesServer.h>
#include <LogService.h>
#include <ArduinoJson.h>
#include <string.h>

namespace diamon {

NodesServer::NodesServer(INetService *netService) :
	_netService(netService){

//	_devices.push_back(R"({"id" : "1", "name" : "Post 1", "state" : "Free"})");


	_netService->OnReceiveEvent += METHOD_HANDLER(NodesServer::OnReceiveMessage);
}

NodesServer::~NodesServer() {
}

size_t NodesServer::DevicesCount() {
	return _devices.size();
}

DeviceList& NodesServer::DevicesList() {
	return _devices;
}

void NodesServer::RequestState(NetAddress addr, LiftState state) {
	LogService::Log("SetState", "ID - " + addr + "  STATE - " + state.ToString());

	LiftNetMessage msg;

	msg.Event

	_netService->Send(NetMessage(m), addr);
}

void NodesServer::set_status(NetAddress addr, LiftState state) {
	auto old_items_count = _devices.size();
	_devices[addr]._state = state;

	if (old_items_count != _devices.size())
		DeviceAddedEvent(addr, state);
	else
		StateChangedEvent(addr, state);
}

void NodesServer::OnReceiveMessage(NetAddress addr, NetMessage *msg){

	LogService::Log("OnMessage", *msg);

	if (msg->Event == NetEvent::DEVICE_STATUS_CHANGED)
	{
		if (msg->Type == DeviceType::LIFT)
			set_status(addr, ((LiftNetMessage*)msg)->State);
	}
}

} /* namespace diamon */
