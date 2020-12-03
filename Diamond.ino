#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#include <mcp_can.h>
#include <SPI.h>
#include "StringLog.h"

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128]; 


//#define CS_PIN D1     // ESP8266 default SPI pins
//#define CLOCK_PIN D5  // Should work with any other GPIO pins, since the library does not formally
//#define MOSI_PIN D7   // use SPI, but rather performs pin bit banging to emulate SPI communication.
//#define MISO_PIN D6
#define CAN0_INT 4 
MCP_CAN CAN0(5); 


#ifndef APSSID
#define APSSID "Diamond"
#define APPSK  "12345678"
#endif

String Messages = "No messages to display!!!";
StringLog* _log; // Логи скетча
int i = 0;

const char *softAP_ssid = APSSID;
const char *softAP_password = APPSK;

const char *myHostname = "esp8266";

const byte DNS_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(172, 217, 28, 1);
IPAddress netMsk(255, 255, 255, 0);

ESP8266WebServer webServer(80);



String responseHTML = "<!DOCTYPE html>\n\
<html lang='en'>\n\
  <head>\n\
    <meta name='viewport' content='width=device-width'>\n\
    <meta charset='UTF-8'>\n\
  </head>\n\
  <body>\n\
  <form action='/action_page.php'>\n\
  <input type='radio' id='male' name='gender' value='male'>\n\
  <label for='male'>Робота</label><br>\n\
  <input type='radio' id='female' name='gender' value='female'>\n\
  <label for='female'>Готово</label><br>\n\
  <input type='radio' id='other' name='gender' value='other'>\n\
  <label for='other'>Вільно</label><br>\n\
  <input type='radio' id='other' name='gender' value='other'>\n\
  <label for='other'>SOS</label>\n\
</form>\n\
<input type='checkbox' id='other' name='gender' value='other'>\n\
 <span class='slider round'></span>\n\
  <label for='other'>Блок</label>\n\
    <a href=\"socket1On\">\n\
      <button style='width: 100px; height: 40px; font-size: 15px; margin: 10px;'>Робота</button>\n\
    </a>\n\
    <br>\n\
    <a href=\"socket2On\">\n\
      <button style='width: 100px; height: 40px; font-size: 15px; margin: 10px;'>Готово</button>\n\
    </a>\n\
    <br>\n\
    <a href=\"socket3On\">\n\
      <button style='width: 100px; height: 40px; font-size: 15px; margin: 10px;'>Вільно</button>\n\
    </a>\n\
    <br>\n\
    <a href=\"socket4On\">\n\
      <button style='width: 100px; height: 40px; font-size: 15px; margin: 10px;'>SOS</button>\n\
    </a>\n\
    <script>\n\
      setInterval(function ( ) {\n\
        var xhttp = new XMLHttpRequest();\n\
        xhttp.onreadystatechange = function() {\n\
          if (this.readyState == 4 && this.status == 200) {\n\
            document.getElementById('can-log').value = this.responseText;\n\
          }\n\
        };\n\
        xhttp.open('GET', 'readData', true);\n\
        xhttp.send();\n\
      }, 10000);\n\
    </script>\n\
  </body>\n\
</html>";


                      // Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "READDATA"){
//    return readDHTTemperature();  
  }
  return String();
}
void setup() {

_log = new StringLog();

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
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });

  webServer.on("/readData", [](){
    webServer.send(200, "text/html",_log->encodeStr(_log->text()));
    /*webServer.send(200, "text/html", Messages);
    Serial.println("/readData");
    if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
    {
      CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
      
      if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
        sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
      else
        sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
    
      Serial.print(msgString);
    
      if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
        sprintf(msgString, " REMOTE REQUEST FRAME");
        Serial.print(msgString);
      } else {
        for(byte i = 0; i<len; i++){
          sprintf(msgString, " 0x%.2X", rxBuf[i]);
          Serial.print(msgString);
        }
      }
          
      Serial.println();
    }
    //delay(10);*/
  });


  webServer.on("/socket1On", [](){
    webServer.send(200, "text/html", responseHTML);
    Serial.println("/socket1On");
   
    byte data[5] = {0x49, 0xAA, 0x00, 0x00, 0x00};
    byte data1[6] = {0x0A, 0x02, 0x80, 0x00, 0x00, 0x00};

    

    byte sndStat = CAN0.sendMsgBuf(0x291, 0, 5, data);
    byte sndStat1 = CAN0.sendMsgBuf(0x40A, 0, 6, data1);
    if(sndStat && sndStat1 == CAN_OK){
      Serial.println(CAN0.sendMsgBuf(0x291, 0, 5, data));
      Serial.println(CAN0.sendMsgBuf(0x40A, 0, 6, data1));
    } else {
      Serial.println("Error Sending Message...");
    }
    //delay(1000);
  });

  webServer.on("/socket2On", [](){
    webServer.send(200, "text/html", responseHTML);
    Serial.println("/socket2On");

    byte data[5] = {0x89, 0x00, 0x05, 0x00, 0x00};
    byte data1[6] = {0x00, 0x02, 0x80, 0x00, 0x00, 0x00};

    byte sndStat = CAN0.sendMsgBuf(0x291, 0, 5, data);
    byte sndStat1 = CAN0.sendMsgBuf(0x400, 0, 6, data1);
    if(sndStat && sndStat1 == CAN_OK){
      Serial.println(CAN0.sendMsgBuf(0x291, 0, 5, data));
      Serial.println(CAN0.sendMsgBuf(0x400, 0, 6, data1));
    } else {
      Serial.println("Error Sending Message...");
    }
    //delay(1000);
  });


  
  webServer.begin();

// Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_100KBPS, MCP_8MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("MCP2515 Library Receive Example...");
}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
    {
      CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
      
      if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
        sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
      else
        sprintf(msgString, "%.3lX.%1d.", rxId, len);
      _log->print(msgString);
      Serial.print(msgString);
      
    
      if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
        sprintf(msgString, " REMOTE REQUEST FRAME");
        Serial.print(msgString);
      } else {
        for(byte i = 0; i<len; i++){
          sprintf(msgString, " %.2X", rxBuf[i]);
          Serial.print(msgString);
          _log->print(msgString);
        }
      }
          
      Serial.println();
      _log->println(".");
    }
    //delay(10);*/
  //_log->print(i);
  //_log->println(" - Заповнюй лог тут");
  i++;

}
