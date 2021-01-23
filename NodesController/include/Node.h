/*
 * Node.h
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#ifndef NODE_H_
#define NODE_H_

#include <INode.h>
#include <IDevice.h>
#include <vector>

namespace diamon {

class Node: public INode {
	int _id;

	std::vector<IDevice*> _devices;

public:
	Node(int id);
	virtual ~Node();
	
	void AddDevice(IDevice *device);

	void Process();
};

} /* namespace diamon */

#endif /* NODE_H_ */
