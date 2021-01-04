/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <espnow.h>
#include <iomanip>
#include <strstream>
#include <sstream>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK  "thereisnospoon"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

uint8_t broadcastAddress[] = {0x84, 0x0D, 0x8E, 0x85, 0xD9, 0x6C};

const long interval = 10000;
unsigned long previousMillis = 0;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

String MACtoSTR(uint8_t* mac){
	char macStr[18];

	snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return String(macStr);

//	std::string result( stream.);

}

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

// Callback when data is sent
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

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
	Serial.println("Received data: " + String((char*)incomingData)
			+ "    MAC: " + String((char*)mac));

	Serial.println();

	//  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

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
  server.handleClient();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;


    String msg = String("Message from: ") + WiFi.macAddress();

    esp_now_send(broadcastAddress, (uint8_t *) msg.c_str(), msg.length() + 1);
  }
}
