/*
 * IStorage.h
 *
 *  Created on: May 6, 2021
 *      Author: rem
 */

#ifndef INCLUDE_STORAGE_ISTORAGE_H_
#define INCLUDE_STORAGE_ISTORAGE_H_

#include <WString.h>

namespace diamon {

class IStorage {
public:
	virtual ~IStorage() { }

	virtual bool SaveKey(const String &key, const String &value) = 0;
	virtual bool LoadKey(const String &key, String &value) = 0;
};

} /* namespace diamon */

#endif /* INCLUDE_STORAGE_ISTORAGE_H_ */
