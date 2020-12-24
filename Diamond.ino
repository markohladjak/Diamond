#include <ESPAsyncTCP.h>

#include <ESPAsyncWebServer.h>


#include <ESP8266WiFi.h>
#include <DNSServer.h>


//#include <mcp_can.h>
#include <SPI.h>

//#define CS_PIN D1     // ESP8266 default SPI pins
//#define CLOCK_PIN D5  // Should work with any other GPIO pins, since the library does not formally
//#define MOSI_PIN D7   // use SPI, but rather performs pin bit banging to emulate SPI communication.
//#define MISO_PIN D6


#ifndef APSSID
#define APSSID "Diamond"
#define APPSK  "12345678"
#endif

const char *softAP_ssid = APSSID;
const char *softAP_password = APPSK;

const char *myHostname = "esp8266";

const byte DNS_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(172, 217, 28, 1);
IPAddress netMsk(255, 255, 255, 0);

AsyncWebServer webServer(80);


String responseHTML = 
#include "./Server.h"
;


class Workplace
{
public:
  enum class States { Work, Ready, Free, SOS };

  States _state = States::Free;

  States State() { return _state; }
  
  void SetState(States state)
  {
    _state = state;
  }
};

Workplace WP1;

String Page(){
  String str = responseHTML;
  
  str.replace("@{M1State0}", WP1.State() == Workplace::States::Work ? "checked" : "");
  str.replace("@{M1State1}", WP1.State() == Workplace::States::Ready ? "checked" : "");
  str.replace("@{M1State2}", WP1.State() == Workplace::States::Free ? "checked" : "");
  str.replace("@{M1State3}", WP1.State() == Workplace::States::SOS ? "checked" : "");
  
  return str;
  }

void setup() {
  Serial.begin(115200);
  WiFi.begin(APSSID, APPSK);

  Serial.println("");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("DNSServer CaptivePortal example");
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
//  webServer.onNotFound([]() {
//    webServer.send(200, "text/html", Page());
//  });

//  webServer.on("/M1State0", [](){ WP1.SetState(Workplace::States::Work); });
//  webServer.on("/M1State1", [](){ WP1.SetState(Workplace::States::Ready); });
//  webServer.on("/M1State2", [](){ WP1.SetState(Workplace::States::Free); });
//  webServer.on("/M1State3", [](){ WP1.SetState(Workplace::States::SOS); });

//  webServer.send(200, "text/html", Page());

  webServer.begin();
}


void loop() {
  dnsServer.processNextRequest();
//  webServer.handleClient();
}
