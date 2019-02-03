// ESP8266 OTA Example
// Target Hardware:  esp8266 NodeMCU
//
// Uses the ArduinoOTA library (included with ESP8266 board support) to allow sketches
// to support firmware updates over the air (WiFi).
//
// This sketch is a skeleton that does nothing but wait for an OTA update request.
// Normal sketch code can be added to this to enable OTA functionality.
//
// NodeMCU I2C Pins 
//   none
//
// Required libraries and board files:
//    esp8266               install from boards manager
//
// Gadget Reboot

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "Your Network Name";
const char* password = "Your Network Password";

void setup() {

  Serial.begin(9600);

  // set WiFi to station mode and disconnect from an AP if previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to WiFi network
  Serial.println();
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // show WiFi status in serial monitor
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  // place code here
}
