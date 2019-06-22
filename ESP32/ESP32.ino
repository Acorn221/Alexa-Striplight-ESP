#include <Arduino.h>
#include <WiFi.h>
#include "fauxmoESP.h"

#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200


#define stripLightPin           4

#define stripLightID           "Strip Light"

#define LEDC_CHANNEL     0

// 8 bit because we get 8 bits from the alexa
#define LEDC_TIMER  8

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000


void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void setup() {

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    
    // this is the PWM setup for the ESP32
    ledcSetup(LEDC_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER);
    ledcAttachPin(stripLightPin, LEDC_CHANNEL);

    
    wifiSetup();
    fauxmo.setPort(80); 
    fauxmo.enable(true);
    fauxmo.addDevice(stripLightID);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);


        if (strcmp(device_name, stripLightID)==0) {
            digitalWrite(stripLightPin, state ? HIGH : LOW);
            if(state){
              ledcWrite(LEDC_CHANNEL, value);
            } else {
              ledcWrite(LEDC_CHANNEL, 0);
            }
        }

    });

}

void loop() {
    fauxmo.handle();
}
