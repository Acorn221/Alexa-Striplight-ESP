#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

fauxmoESP fauxmo;

#define SERIAL_BAUDRATE     115200


#define stripLightPin           15

#define stripLightID           "Strip Light"



void wifiSetup() {
    WiFi.mode(WIFI_STA);
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void setup() {

    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();
    wifiSetup();
    fauxmo.createServer(true);
    fauxmo.setPort(80);
    fauxmo.enable(true);
    fauxmo.addDevice(stripLightID);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        if (strcmp(device_name, stripLightID)==0) {
            if(state){
              if(value <= 2){ // this is to create a subtle affect with the strip lights
                analogWrite(stripLightPin, 1);
              } else {
                analogWrite(stripLightPin, value*4); // otherwise, set it to whatever
              }
            } else {
              analogWrite(stripLightPin, 0);
            }
        }

    });

}

void loop() {
    fauxmo.handle();
}
