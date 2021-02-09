/*
 * utils.cpp
 *
 *  Created on: Feb 9, 2021
 *      Author: rem
 */

#include <utils.h>

namespace diamon {

utils::utils() {
	// TODO Auto-generated constructor stub

}

utils::~utils() {
	// TODO Auto-generated destructor stub
}

String utils::ESPEFuseMacStr()
{
	uint64_t mac = ESP.getEfuseMac();
	char mac_str[18];
	uint8_t *m = (uint8_t*)&mac;

	sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", m[5], m[4], m[3], m[2], m[1], m[0]);

//	Serial.printf("%012" PRIx64 "\n", mac);

//	Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(mac>>32));//print High 2 bytes
//	Serial.printf("%08X\n",(uint32_t)mac);//print Low 4bytes.

	return mac_str;
}

} /* namespace diamon */