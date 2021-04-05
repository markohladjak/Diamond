#include "PermanentAP.h"
#include <WiFi.h>

const char *ssid = "DiamandNC";
const char *password = "nodesAdmin";

int freq = 5000;
int ledChannel = 0;
int resolution = 8;

void wifiEventCallback(WiFiEvent_t event, WiFiEventInfo_t info);
void fade_task(void *);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(~0);
  Serial.println("Configuring access point...");

  WiFi.onEvent(wifiEventCallback, SYSTEM_EVENT_AP_START);

  WiFi.softAP(ssid, password, 1, 0, 10);

  xTaskCreate(fade_task, "MPRX", 3072, NULL, 5, NULL);
}

void wifiEventCallback(WiFiEvent_t event, WiFiEventInfo_t info)
{
	switch(event) {
	case SYSTEM_EVENT_SCAN_DONE:
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		break;
	case SYSTEM_EVENT_STA_LOST_IP:
		break;
	case SYSTEM_EVENT_AP_START: {
		  WiFi.softAPConfig(IPAddress(10,10,10,1), IPAddress(10,10,10,1), IPAddress(255,255,255,0));

		  IPAddress myIP = WiFi.softAPIP();
		  Serial.print("AP IP address: ");
		  Serial.println(myIP);
		}
		break;
	default:
		break;
	}
}

void fade_task(void *) {
	ledcSetup(ledChannel, freq, resolution);
	ledcAttachPin(LED_BUILTIN, ledChannel);

	while(1) {
		for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
			ledcWrite(ledChannel, dutyCycle);
			delay(5);
		}

		for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
			ledcWrite(ledChannel, dutyCycle);
			delay(5);
		}
	}
}

void loop() {
	vTaskDelay(~0);
}
