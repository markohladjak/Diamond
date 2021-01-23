/*
 * Lift.h
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#ifndef LIFT_H_
#define LIFT_H_

#include <IController.h>
#include <IDevice.h>

namespace diamon {


class Lift: public IDevice {
public:
	Lift();
	~Lift() {};

	bool ExecuteCommand(ICommand* command) { return true; }
	DeviceState GetState() { return 0; }
};

} /* namespace diamon */

#endif /* LIFT_H_ */
