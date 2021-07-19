/*
 * utils.cpp
 *
 *  Created on: Feb 9, 2021
 *      Author: rem
 */

#include <utils.h>
#include <version.h>
#include <LogService.h>

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

//String utils::UINT64ToString(uint64_t val) {
//	  uint32_t Vh = (unsigned long)((val & 0xFFFFFFFF00000000) >> 32 );
//	  uint32_t Vl = (unsigned long)((val & 0x00000000FFFFFFFF));
//
//	  return String(Vh, HEX) + String(Vl, HEX);
//}

String utils::UINT64ToString(uint64_t val) {
	char buf[17];
	uint8_t *ref = (uint8_t*)&val;

	sprintf(buf, "%02x%02x%02x%02x%02x%02x%02x%02x", ref[7], ref[6], ref[5], ref[4], ref[3], ref[2], ref[1], ref[0]);

	return buf;
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
#ifdef VERSION
	return VERSION;
#else
	return "0.0.0.0";
#endif
}

KeyValuePairsT utils::ParseKayValue(std::string str)
{
	char s[str.size()];
	strcpy(s, str.c_str());

	char *exp, *s_ptr = s;
	char *p1, *p2;

	KeyValuePairsT res;

	while ((exp = strtok_r(s_ptr, " ;", &p1)))
	{
		auto key = strtok_r(exp, "=", &p2);
		auto value = strtok_r(NULL, "=", &p2);

		res[key] = value;

		s_ptr = NULL;
	}

	return res;
}

} /* namespace diamon */
