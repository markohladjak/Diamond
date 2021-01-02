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

const char *ssid = "Marko";
const char *password = "mmmmmmmm";

AsyncWebServer webServer(80);

String responseHTML = 
#include "./test.h"
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


  Serial.println(str);

  return str;
  }

void initWiFi();

void setup() {
  Serial.begin(115200);

  Serial.println("Work");

  initWiFi();
  
//  webServer.onNotFound([]() {
//    webServer.send(200, "text/html", Page());
//  });
//
//  webServer.on("/M1State0", [](){ WP1.SetState(Workplace::States::Work); });
//  webServer.on("/M1State1", [](){ WP1.SetState(Workplace::States::Ready); });
//  webServer.on("/M1State2", [](){ WP1.SetState(Workplace::States::Free); });
//  webServer.on("/M1State3", [](){ WP1.SetState(Workplace::States::SOS); });

  webServer.onNotFound([](AsyncWebServerRequest *request){
    Serial.println("onNotFound");

    request->send_P(200, "text/html", Page().c_str());
  });

  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("on");

    request->send_P(200, "text/html", Page().c_str());
  });
  
  webServer.begin();
}


void loop() {
//  webServer.handleClient();
//    Serial.println("loop");
}

void initWiFi() {
    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("WiFi shield not present");
      while (true);
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    //IPAddress ip(192,168,0,35);
    //IPAddress gateway(192,168,0,1);
    //IPAddress subnet(255,255,255,0);
    
    //WiFi.config(ip, gateway, subnet);

    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}
