/*
 * Device.cpp
 *
 *  Created on: May 6, 2021
 *      Author: rem
 */

#include <IDevice.h>
#include <Net/ComData/DeviceInfo.h>

namespace diamon {


IDevice::IDevice(IStorage *storage)
		: _storage(storage)
{
}

void IDevice::load_data() {
	if (!_storage) return;

	_storage->LoadKey(MSG_TAG_DEVICE_INFO_NAME, _name);
}

void IDevice::SetName(const String &name) {
	xSemaphoreTake(_state_mutex, portMAX_DELAY);

	_name = name;

	if (_storage)
		_storage->SaveKey(MSG_TAG_DEVICE_INFO_NAME, name);

	NameChangedEvent(name);

	xSemaphoreGive(_state_mutex);
}

}
