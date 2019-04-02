/*********************************************************************
  This is an example for reading the temperature out of a
  DS18B20 sensor and displaying the result in the Serial monitor 
  of the Arduino IDE.

  Temperature is displayed in degrees C and F.

  Gadget Reboot

*********************************************************************/

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2                // DS18B20 data wire is connected to input 2

DeviceAddress thermometerAddress;     // custom array type to hold 64 bit device address

OneWire oneWire(ONE_WIRE_BUS);        // create a oneWire instance to communicate with temperature IC
DallasTemperature tempSensor(&oneWire);  // pass the oneWire reference to Dallas Temperature

void setup()   {

  Serial.begin(9600);
                      
  Serial.println("DS18B20 Temperature IC Test");
  Serial.println("Locating devices...");
  tempSensor.begin();                         // initialize the temp sensor

  if (!tempSensor.getAddress(thermometerAddress, 0))
    Serial.println("Unable to find Device.");
  else {
    Serial.print("Device 0 Address: ");
    printAddress(thermometerAddress);
    Serial.println();
  }

  tempSensor.setResolution(thermometerAddress, 9);      // set the temperature resolution (9-12)
}


void loop() {

  tempSensor.requestTemperatures();                      // request temperature sample from sensor on the one wire bus
  displayTemp(tempSensor.getTempC(thermometerAddress));  // show temperature on OLED display

  delay(500);                                            // update readings every half a second
}

void displayTemp(float temperatureReading) {             // temperature comes in as a float with 2 decimal places

  // show temperature °C
  Serial.print(temperatureReading);      // serial debug output
  Serial.print("°");
  Serial.print("C  ");

  // show temperature °F
  Serial.print(DallasTemperature::toFahrenheit(temperatureReading));     // serial debug output
  Serial.print("°");
  Serial.println("F");
}


// print device address from the address array
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
