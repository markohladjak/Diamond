/*
 * utils.cpp
 *
 *  Created on: Feb 9, 2021
 *      Author: rem
 */

#include <utils.h>
#include <version.h>

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
	auto m = (uint8_t*)&mac;

	sprintf(mac_str, MACSTR, MAC2STR(m));

//	Serial.printf("%012" PRIx64 "\n", mac);

//	Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(mac>>32));//print High 2 bytes
//	Serial.printf("%08X\n",(uint32_t)mac);//print Low 4bytes.

	return mac_str;
}

uint64_t utils::GenerateDeviceID(byte id) {
	return ((ESP.getEfuseMac()) << 16) + id;
}

String utils::UINT64ToString(uint64_t val) {
	  uint32_t Vh = (unsigned long)((val & 0xFFFFFFFF00000000) >> 32 );
	  uint32_t Vl = (unsigned long)((val & 0x00000000FFFFFFFF));

	  return String(Vh, HEX) + String(Vl, HEX);
}

uint64_t utils::StringToUINT64(String val) {
	const char *str = val.c_str();
	char *end = (char *)val.c_str() + val.length();

	return strtoull(str, &end, 16);
}

//#define tskKERNEL_VERSION_NUMBER "V8.2.0"
//#define tskKERNEL_VERSION_MAJOR 8
//#define tskKERNEL_VERSION_MINOR 2
//#define tskKERNEL_VERSION_BUILD 0

String utils::GetVersion() {
	return VERSION;
}

} /* namespace diamon */
