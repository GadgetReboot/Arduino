// Using the DHT2x Temperature/Humidity sensor with a 16x2 LCD via I2C
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
#include <Wire.h>
#include <hd44780.h>                       // LCD hd44780 library
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

#define DHTPIN 2        // digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // can be DHT11, DHT21, DHT22  

// create a DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

// create lcd object: auto locate & auto config gpio expander chip
hd44780_I2Cexp lcd;  

void setup(void) {

  Serial.begin(9600);
  Serial.println("DHT Temperature/Humidity Sensor");
  Serial.println();

  // initialize the DHT sensor
  dht.begin();
  
   // initialize lcd
  lcd.begin(16, 2);
  lcd.clear();
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

    lcd.setCursor(0, 0);
    lcd.print("T: ");
    lcd.print(tempC, 1);
    lcd.print((char)223);
    lcd.print("C ");
    lcd.print(tempF, 1);
    lcd.print((char)223);
    lcd.print("F");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(humidity, 1);
    lcd.print("%RH");
  }

  // DHT22 should only be read once every two seconds
  delay(2000);

}  // end void loop()
