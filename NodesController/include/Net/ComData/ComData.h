/*
 * ComData.h
 *
 *  Created on: Apr 20, 2021
 *      Author: rem
 */

#ifndef NET_COMDATA_H_
#define NET_COMDATA_H_

#include <WString.h>
#include <IDevice.h>
#include <ArduinoJson.h>
#include <memory>

namespace diamon {

class CommunicationData;

typedef std::shared_ptr<CommunicationData> comdata_sptr_t;

class CommunicationData
{
protected:
	virtual void serialize(JsonDocument &json) const = 0;
	virtual bool parse(JsonDocument &json) = 0;

public:
	virtual ~CommunicationData();

	virtual String getType() const = 0;
	virtual String dataType() const = 0;

//	virtual DeviceType getDeviceType() = 0;
//	virtual bool deserialize(String str) const = 0;

	bool FromJson(const String &jsonString);
	String ToJson();
};

} /* namespace diamon */

#endif /* NET_COMDATA_H_ */
