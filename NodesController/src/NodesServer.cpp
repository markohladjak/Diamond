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
	_netService->OnLayoutChangedEvent += METHOD_HANDLER(NodesServer::OnNetworkChanged);

	request_report_all();
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
	LogService::Log("SetState", "ID - " + addr.ToString() + "  STATE - " + state.ToString());

	LiftNetMessage msg;

	msg.Event = NetEvent::REQUEST_DEVICE_STATE;
	msg.State = state;

	_netService->Send(msg, addr);
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

void NodesServer::OnNetworkChanged() {
	request_report_all();
}

void NodesServer::request_report_all() {
	LogService::Log("request_report_all:","");

	NetAddress addr = NetAddress::BROADCAST;
	NetMessage msg;

	msg.Event = NetEvent::DEVICE_STATE_REPORT;
	msg.Type = DeviceType::LIFT;

	_netService->Send(msg, addr);
}

} /* namespace diamon */
