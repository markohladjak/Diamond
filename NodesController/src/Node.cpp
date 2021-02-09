/*
 * Node.cpp
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#include <Node.h>
#include <LogService.h>

namespace diamon {

Node::Node(uint64_t id) {
	_id = id;

	_logService = new LogService();
}

Node::~Node() {
}

void Node::AddDevice(IDevice *device) {
	_devices.push_back(device);
}

void Node::Process() {
	for (auto device: _devices)
		device->update();
}

} /* namespace diamon */
