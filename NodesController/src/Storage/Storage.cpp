/*
 * Storage.cpp
 *
 *  Created on: May 6, 2021
 *      Author: rem
 */

#include <Storage/Storage.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "nvs_flash.h"


namespace diamon {


void Storage::init() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

void Storage::open_nvs(const char* name) {
	printf("\n");
	printf("Opening Non-Volatile Storage (NVS) handle... ");

	esp_err_t err = nvs_open(name, NVS_READWRITE, &_handle);

	if (err != ESP_OK) {
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else
		printf("Done\n");
}


Storage::Storage(const char* Namespace) {
	init();
	open_nvs(Namespace);
}

Storage::~Storage() {
	if (_handle)
		nvs_close(_handle);
}

bool Storage::LoadKey(const String &key, String &value) {
	if (!_handle) return false;

	char buf[32] = { 0 };
	unsigned int len = 32;

	esp_err_t err = nvs_get_str(_handle, key.c_str(), buf, &len);

	printf("LoadKey: %s - %s     ", key.c_str(), buf);

	switch (err) {
        case ESP_OK:
            printf("Done\n");
            value = buf;
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            return false;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
            return false;
    }

    return true;
}

bool Storage::SaveKey(const String &key, const String &value) {
	if (!_handle) return false;

	esp_err_t err = nvs_set_str(_handle, key.c_str(), value.c_str());

	printf("SaveKey: %s - %s     ", key.c_str(), value.c_str());
	printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

	printf("Committing updates in NVS ... ");

	err = nvs_commit(_handle);

	printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

	return err == ESP_OK;
}

} /* namespace diamon */
