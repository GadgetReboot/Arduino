/****************************************************************
  SHT21 Temperature/Humidity Sensor Demo

   An example sketch that reads the sensor data and prints the
   temperature in degrees C and relative humidity %RH to the 
   serial port.

   Data will appear in the serial monitor of the Arduino IDE and
   if an external UART is connected, data can be read and parsed
   on other hardware.

   Target hardware:  ESP8266

   Gadget Reboot

***************************************************************/

#include <Wire.h>
#include "SHT21.h"

SHT21 SHT21;

void setup() {

  // initialize serial port and SHT21 sensor
  Serial.begin(9600);
  SHT21.begin();
  
}  // end void setup()

void loop() {

  // send temperature and humidity out over the UART
  // formatted with text that can be parsed on receiving side
  // format:  "nnnHumidity" or "nnnTemperature"  where nnn is a floating number
  Serial.print(SHT21.getHumidity());
  Serial.println("Humidity");
  delay(1000);
  
  Serial.print(SHT21.getTemperature());
  Serial.println("Temperature");
  delay(1000);
  
}  // end void loop()



