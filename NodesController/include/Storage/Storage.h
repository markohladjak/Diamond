/*
 * Storage.h
 *
 *  Created on: May 6, 2021
 *      Author: rem
 */

#ifndef STORAGE_STORAGE_H_
#define STORAGE_STORAGE_H_

#include "IStorage.h"
#include "nvs.h"

namespace diamon {

class Storage : public IStorage {
	nvs_handle _handle = 0;

	void init();
	void open_nvs(const char *name);

public:
	Storage(const char* Namespace);
	~Storage() override;

	virtual bool LoadKey(const String &key, String &value);
	virtual bool SaveKey(const String &key, const String &value);
};

} /* namespace diamon */

#endif /* STORAGE_STORAGE_H_ */
