// ESP8266 OTA Example
// Target Hardware:  esp8266 NodeMCU
//
// Uses the ArduinoOTA library (included with ESP8266 board support) to allow sketches
// to support firmware updates over the air (WiFi).
// This sketch flashes an LED to show how OTA works in conjunction with a sketch.
//
// NodeMCU I2C Pins 
//    LED:  D1
//
// Required libraries and board files:
//    esp8266               install from boards manager
//
// Gadget Reboot


#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#define ledPin D1  // led to blink

const char* ssid = "Your Network Name";
const char* password = "Your Network Password";

void setup() {
  Serial.begin(9600);

  // start with led OFF
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);   

  // join WiFi network
  Serial.println();
  Serial.println("Joining WiFI...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("GR-ESP8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  
  // initialize OTA capability
  ArduinoOTA.begin();
}

void loop() {
  // handle any OTA requests
  ArduinoOTA.handle();

  // normal sketch code
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}
