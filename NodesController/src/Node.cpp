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

namespace diamon {

#define SERVER_ADDRESS NetAddress::SERVER

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
		break;
	case DeviceType::DOOR:
		break;
	}

	OnLiftStateChanged(((Lift*)device)->GetState());
}

void Node::OnLiftStateChanged(LiftState state) {
	LiftNetMessage msg;

	msg.Event = NetEvent::DEVICE_STATUS_CHANGED;
	msg.State = state;

	_devices.begin()->second->Send(msg, SERVER_ADDRESS);
}

void Subscribe(IDevice* device)
{
//			lift->StateChangedEvent += METHOD_HANDLER(Node::OnLiftStateChanged);
}

void Node::Process() {
	for (auto device: _devices)
		device.first->update();
}


} /* namespace diamon */
