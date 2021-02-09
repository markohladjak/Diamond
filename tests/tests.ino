//
// A simple server implementation with regex routes:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

// Add buildflag ASYNCWEBSERVER_REGEX to enable the regex support

// For platformio: platformio.ini:
//  build_flags =
//      -DASYNCWEBSERVER_REGEX

// For arduino IDE: create/update platform.local.txt
// Windows: C:\Users\(username)\AppData\Local\Arduino15\packages\espxxxx\hardware\espxxxx\{version}\platform.local.txt
// Linux: ~/.arduino15/packages/espxxxx/hardware/espxxxx/{version}/platform.local.txt
//
// compiler.cpp.extra_flags=-DASYNCWEBSERVER_REGEX=1

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <inttypes.h>
#include <Update.h>

using namespace fs;

AsyncWebServer server(80);
AsyncEventSource events("/events");

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

String html;
#include "index.html"
;

String ESPEFuseMacStr();

void setup() {

    Serial.begin(115200);
//    WiFi.mode(WIFI_STA);
    WiFi.softAP("AP1");



//    WiFi.begin(ssid, password);
//    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
//        Serial.printf("WiFi Failed!\n");
//        return;
//    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", html);
    });

    // Send a GET request to <IP>/sensor/<number>
    server.on("^\\/sensor\\/([0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String sensorNumber = request->pathArg(0);
        request->send(200, "text/plain", "Hello, sensor: " + sensorNumber);
    });

	// Send a GET request to <IP>/sensor/<number>/action/<action>
    server.on("^\\/sensor\\/([0-9]+)\\/action\\/([a-zA-Z0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String sensorNumber = request->pathArg(0);
        String action = request->pathArg(1);
        request->send(200, "text/plain", "Hello, sensor: " + sensorNumber + ", with action: " + action);
    });

    server.on("/click", HTTP_GET, [] (AsyncWebServerRequest *request) {

    	Serial.println(request->url());
    	request->send(200, "text/plain", "Hello, sensor: ");
    });


    server.onNotFound(notFound);

    // Handle Web Server Events
    events.onConnect([](AsyncEventSourceClient *client){
      if(client->lastId()){
        Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
      }
      // send event with message "hello!", id current millis
      // and set reconnect delay to 1 second
      client->send("hello!", NULL, millis(), 10000);
    });
    server.addHandler(&events);


    server.begin();

    SPIFFSFS spiffs;

    spiffs.begin(true);

    Serial.print("------------- size of FFS: ");
    Serial.println(spiffs.totalBytes());


    File file = SPIFFS.open("/boards.txt");

    if (file)
    	Serial.println("File opened");

    Serial.println(spiffs.exists("/boards.txt"));

//    DynamicJsonDocument doc(1000);

    Serial.print("ESP.getHeapSize(): ");
    Serial.println(ESP.getHeapSize());
    Serial.print("ESP.getFlashChipSize(): ");
    Serial.println(ESP.getFlashChipSize());
    Serial.print("ESP.getCpuFreqMHz(): ");
    Serial.println(ESP.getCpuFreqMHz());
    Serial.print("ESP.getSdkVersion(): ");
    Serial.println(ESP.getSdkVersion());
    Serial.print("ESP.getEfuseMac(): ");
    Serial.println(ESPEFuseMacStr());
//    Serial.print("");
//    Serial.println(ESP.getHeapSize());
}

String ESPEFuseMacStr()
{
	uint64_t mac = ESP.getEfuseMac();
	char mac_str[18];
	uint8_t *m = (uint8_t*)&mac;

	sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", m[0], m[1], m[2], m[3], m[4], m[5]);

	Serial.printf("%012" PRIx64 "\n", mac);


	Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(mac>>32));//print High 2 bytes
	Serial.printf("%08X\n",(uint32_t)mac);//print Low 4bytes.

	return mac_str;
}

unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

void loop() {
	if ((millis() - lastTime) > timerDelay) {
//	    Serial.printf("Pressure = %.2f hPa \n", pressure);
//	    Serial.println();

	    // Send Events to the Web Server with the Sensor Readings

		auto v1 = rand() % 100;

	    events.send("ping",NULL,millis());
	    events.send(String(v1).c_str(),"msg",millis());

	    lastTime = millis();
	  }

}
