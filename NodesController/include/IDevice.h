/*
 * FFF.h
 *
 *  Created on: Jan 16, 2021
 *      Author: rem
 */

#ifndef INCLUDE_IDEVICE_H_
#define INCLUDE_IDEVICE_H_

#include <ICommand.h>

namespace diamon {

typedef int DeviceState;

class IDevice {
public:
		virtual bool ExecuteCommand(ICommand* command) = 0;
		virtual DeviceState GetState() = 0;

		virtual ~IDevice() {};
};

} /* namespace diamon */

#endif /* INCLUDE_IDEVICE_H_ */
