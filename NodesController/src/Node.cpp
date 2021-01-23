/*
 * Node.cpp
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#include <Node.h>

namespace diamon {

Node::Node(int id) {
	_id = id;
}

Node::~Node() {
}

void Node::AddDevice(IDevice *device) {
	_devices.push_back(device);
}

void Node::Process() {
}

} /* namespace diamon */
