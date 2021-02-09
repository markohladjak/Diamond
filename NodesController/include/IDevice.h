/*
 * FFF.h
 *
 *  Created on: Jan 16, 2021
 *      Author: rem
 */

#ifndef INCLUDE_IDEVICE_H_
#define INCLUDE_IDEVICE_H_

#include <ICommand.h>
#include "IInLoop.h"

namespace diamon {

class DeviceState { };


class IDevice: public IInLoop {
public:
		virtual bool ExecuteCommand(ICommand* command) = 0;

		virtual ~IDevice() {};
};

} /* namespace diamon */

#endif /* INCLUDE_IDEVICE_H_ */
