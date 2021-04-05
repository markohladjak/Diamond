/*
 *
 *  Created on: Mar 31, 2021
 *      Author: rem
 */

#include <Net/FA/FirmwareUpdate.h>
#include "SPIFFS.h"
#include <ESPmDNS.h>
#include <Update.h>
#include <utils.h>

namespace diamon {

FirmwareUpdate::FirmwareUpdate() {
	SPIFFS.begin();

	_server = new AsyncWebServer(80);

	setup();

	_server->begin();
}

FirmwareUpdate::~FirmwareUpdate() {
	_server->end();

	delete _server;

	SPIFFS.end();
}

void FirmwareUpdate::setup(void) {
	_ssid = ssidName();

	WiFi.softAP(_ssid.c_str(), _password.c_str());

//	WiFi.onEvent(wifiEventCallback, SYSTEM_EVENT_STA_CONNECTED);

	Serial.println("");

	if (!MDNS.begin(_host.c_str()))  //http://esp32.local
		Serial.println("Error setting up MDNS responder!");
	else
		Serial.println("mDNS responder started");

	_server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/html", loginIndex);
	});

	_server->on("/serverIndex", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/html", serverIndex);
	});

	_server->on("/src/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/src/jquery-3.3.1.min.js");
	});

	_server->on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
		request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
		ESP.restart();
	}
	, uploadHandler);
}

void FirmwareUpdate::uploadHandler(AsyncWebServerRequest *request,
		const String &filename, size_t index, uint8_t *data, size_t len,
		bool final) {
	if(!index){
		Serial.printf("Upload Start: %s\n", filename.c_str());
		if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH, LED_BUILTIN, HIGH)) {
			Update.printError(Serial);
		}
	}

	Serial.printf(".");

	if (Update.write(data, len)	!= len) {
		Update.printError(Serial);
	}

	if (final) {
		Serial.println();

		if (Update.end(true)) { //true to set the size to the current progress
			Serial.printf("Update Success: %u\nRebooting...\n", index+len);
		} else {
			Update.printError(Serial);
		}
	}
}

String FirmwareUpdate::ssidName() {
	return FA_SSID_PREFIX + utils::ESPEFuseMacStr();
}

void FirmwareUpdate::wifiEventCallback(WiFiEvent_t event, WiFiEventInfo_t info)
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
	default:
		break;
	}
}

const char* FirmwareUpdate::loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

/*
 * Server Index Page
 */

const char* FirmwareUpdate::serverIndex =
"<script src='/src/jquery-3.3.1.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

} /* namespace diamon */
