#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <espnow.h>
#include <iomanip>
#include <strstream>
#include <sstream>

uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x0E, 0x4F, 0xB6};

const long interval = 100;
unsigned long previousMillis = 0;

String MACtoSTR(uint8_t* mac){
	char macStr[18];

	snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return String(macStr);
}

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }

  Serial.println("to: " + MACtoSTR(mac_addr));
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
	Serial.println("Received data: " + String((char*)incomingData)
			+ "    MAC: " + MACtoSTR(mac));

	Serial.println();

}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());


  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  esp_now_register_send_cb(OnDataSent);

  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    String msg = String("Message from: ") + WiFi.macAddress();

    esp_now_send(broadcastAddress, (uint8_t *) msg.c_str(), msg.length() + 1);
  }
}
