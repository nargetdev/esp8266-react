#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <WiFiSettingsService.h>
#include <WiFiStatus.h>
#include <WiFiScanner.h>
#include <APSettingsService.h>
#include <NTPSettingsService.h>
#include <NTPStatus.h>
#include <OTASettingsService.h>
#include <APStatus.h>
#include <ForceARP.h>

#include <lightstrip/LightStripService.h>

#define SERIAL_BAUD_RATE 115200

AsyncWebServer server(80);

WiFiSettingsService wifiSettingsService = WiFiSettingsService(&server, &SPIFFS);
APSettingsService apSettingsService = APSettingsService(&server, &SPIFFS);
NTPSettingsService ntpSettingsService = NTPSettingsService(&server, &SPIFFS);
OTASettingsService otaSettingsService = OTASettingsService(&server, &SPIFFS);

LightStripService lightStripService = LightStripService(&server);

WiFiScanner wifiScanner = WiFiScanner(&server);
WiFiStatus wifiStatus = WiFiStatus(&server);
NTPStatus ntpStatus = NTPStatus(&server);
APStatus apStatus = APStatus(&server);

void setup() {
    // Disable wifi config persistance
    WiFi.persistent(false);

    Serial.begin(SERIAL_BAUD_RATE);
    SPIFFS.begin();

    // start services
    ntpSettingsService.begin();
    otaSettingsService.begin();
    apSettingsService.begin();
    wifiSettingsService.begin();

    // start the light strip
    lightStripService.begin();

    // Serving static resources from /www/
    server.serveStatic("/js/", SPIFFS, "/www/js/");
    server.serveStatic("/css/", SPIFFS, "/www/css/");
    server.serveStatic("/fonts/", SPIFFS, "/www/fonts/");
    server.serveStatic("/app/", SPIFFS, "/www/app/");

    // Serving all other get requests with "/www/index.htm"
    // OPTIONS get a straight up 200 response
    server.onNotFound([](AsyncWebServerRequest *request) {
    	if (request->method() == HTTP_GET) {
        request->send(SPIFFS, "/www/index.html");
      } else if (request->method() == HTTP_OPTIONS) {
		    request->send(200);
      } else {
    		request->send(404);
    	}
    });

    // Disable CORS if required
    #if defined(ENABLE_CORS)
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
    #endif

    server.begin();
}

void loop() {
  apSettingsService.loop();
  ntpSettingsService.loop();
  otaSettingsService.loop();

  // service the light strip
  lightStripService.loop();

  forceARPLoop();
}
