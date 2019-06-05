// Using the DHT2x Temperature/Humidity sensor with the serial monitor
//
// Based on Adafruit sensor library example sketches
// REQUIRES the following libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
//
// Tested with Arduino IDE v1.8.9 
//             Adafruit Unified Sensor v1.0.3
//             DHT Sensor Library v1.3.4
//             HD44780 v1.0.1
//
//  Gadget Reboot

#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// create a DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

void setup(void) {

  Serial.begin(9600);
  Serial.println("DHT Temperature/Humidity Sensor");
  Serial.println();

  // initialize the DHT sensor
  dht.begin();
}

void loop() {

  // reading temperature or humidity takes about 250 milliseconds
  // read humidity
  float humidity = dht.readHumidity();

  // read temperature:  true=degF false=degC
  float tempC = dht.readTemperature(false);
  float tempF = dht.readTemperature(true);

  // Check if any reads failed
  if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(tempC);
    Serial.print(F("°C "));
    Serial.print(tempF);
    Serial.print(F("°F "));
    Serial.println();
  }

  // DHT22 should only be read once every two seconds
  delay(2000);

}  // end void loop()
