/*
 * Node.cpp
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#include <Node.h>
#include <LogService.h>
#include <Lift.h>
#include <Events.h>
#include <Helpers/JsonHelper.h>
#include <Net/NetAddress.h>
#include <utils.h>
#include <Net/NetMessage.h>
#include <Net/ComData/ComTypes.h>

namespace diamon {

Node::Node() {
}

Node::~Node() {
}

void Node::AddDevice(IDevice *device, INetService *netService) {
	_devices[device] = netService;

	switch (device->Type())
	{
	case DeviceType::LIFT:
		((Lift*)device)->StateChangedEvent += METHOD_HANDLER(Node::OnLiftStateChanged);
		device->NameChangedEvent += METHOD_HANDLER(Node::OnDeviceNameChanged);

		netService->OnReceiveEvent += METHOD_HANDLER(Node::OnNetMessage);
		netService->OnRootAddressObtainedEvent += METHOD_HANDLER(Node::OnRootObtained);
//		netService->OnConnectedEvent += METHOD_HANDLER(Node::OnNetworkConnected);
		break;
	case DeviceType::DOOR:
		break;
	}

//	OnLiftStateChanged(((Lift*)device)->GetState());
}

void Node::SendName(const String &name) {
	auto le = new DDeviceNameChangedEvent;
	le->NewName = name;

	_devices.begin()->second->Send(le, NetAddress::SERVER);
}

void Node::SendState(LiftState state) {
	auto le = new LiftStateChangedEvent;
	le->State = state;

	_devices.begin()->second->Send(le, NetAddress::SERVER);
}

void Node::SendLiftInfo(LiftState state, String name) {
	auto le = new LiftInfo;
	le->Name = name;
	le->State = state;

	_devices.begin()->second->Send(le, NetAddress::SERVER);
}

void Node::OnLiftStateChanged(LiftState state) {
	SendState(state);
}

void Node::OnDeviceNameChanged(const String &name) {
	SendName(name);
}

void Subscribe(IDevice* device)
{
//			lift->StateChangedEvent += METHOD_HANDLER(Node::OnLiftStateChanged);
}

void PrintTasksState()
{
    uint8_t _rx_buf[3000] = { 0 };

    // vTaskGetRunTimeStats((char*)_rx_buf);
    printf("Name          State   Priority  Stack  Num\n\
******************************************\n");

    vTaskList((char*)_rx_buf);
    printf((char*)_rx_buf);

    printf("\nTasks count: %d\n", uxTaskGetNumberOfTasks());
}

void Node::OnNetMessage(NetAddress form, NetMessage *message) {
	LogService::Log("Node::OnNetMessage", *message);

	if (message->Context().getType() == "DeviceGetInfoCommand")
	{
		auto state = ((Lift*)(_devices.begin()->first))->GetState();
		auto name = ((Lift*)(_devices.begin()->first))->GetName();

		SendLiftInfo(state, name);
	}

	if (message->Context().getType() == "SetLiftStateCommand") {
		((Lift*)(_devices.begin()->first))->SetState(   ( (SetLiftStateCommand&) (message->Context()) ).State  );
//		PrintTasksState();
	}

	if (message->Context().getType() == "SetDeviceNameCommand")
		((IDevice*)(_devices.begin()->first))->SetName(   ( (SetDeviceNameCommand&) (message->Context()) ).Name  );
}

void Node::OnNetworkConnected() {
	LogService::Println("Node::OnNetworkConnected()");

	OnRootObtained();
}

void Node::OnRootObtained() {
	LogService::Println("Node::OnRootObtained");

	auto state = ((Lift*)(_devices.begin()->first))->GetState();
	auto name = ((Lift*)(_devices.begin()->first))->GetName();

	SendLiftInfo(state, name);
}

void Node::Process() {
	for (auto device: _devices)
		device.first->update();
}


} /* namespace diamon */
