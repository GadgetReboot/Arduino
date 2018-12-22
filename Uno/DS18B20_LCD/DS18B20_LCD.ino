// ----------------------------------------------------------------------------
// Sketch based on LCD demonstration
// by Bill Perry bperrybap@opensource.billsworld.billandterrie.com
// and various DS18B20 Onewire/Dallas library code snippets
//
// This example code is unlicensed and is released into the public domain
//
// Gadget Reboot
// ----------------------------------------------------------------------------
//
// This sketch is for LCDs with PCF8574 backpacks/interfaces (serial I2C control)
// as well as direct wiring to Arduino digital outputs (parallel control).
// The "lcd" object interfaces with the I2C display
// The "lcdParallel" object interfaces with the direct Arduino pin controlled display
//
// The DS18B20 digital temperature sensor reading is displayed in degrees C
// on one LCD and in degrees F on the other LCD to demonstrate how to use the
// sensor as well as the LCDs in serial or parallel mode.
//
// WARNING:
//	Use caution when using 3v only processors like arm and ESP8266 processors
//	when interfacing with 5v modules as not doing proper level shifting or
//	incorrectly hooking things up can damage the processor.
//
// While not all hd44780 LCD drivers use the same pinout, here is the most common:
// pin 1 is the pin closest to the edge of the PCB
//  1 - LCD gnd
//  2 - VCC (5v)
//  3 - Vo Contrast Voltage
//  4 - RS Register Select (rs)
//  5 - Read/Write
//  6 - Enable (en)
//  7 - Data 0 (db0) ----
//  8 - Data 1 (db1)     |-------- Not used in 4 bit mode
//  9 - Data 2 (db2)     |
// 10 - Data 3 (db3) ----
// 11 - Data 4 (db4)
// 12 - Data 5 (db5)
// 13 - Data 6 (db6)
// 14 - Data 7 (db7)
// 15 - Backlight Anode (+5v)
// 16 - Backlight Cathode (Gnd)
//
//
// Requires the HD44780 library for the LCD
// Requires the OneWire and DallasTemperature libraries for the DS18B20

#include <Wire.h>
#include <hd44780.h>                       // LCD hd44780 library
#include <hd44780ioClass/hd44780_I2Cexp.h> // I2C expander i/o class 
#include <hd44780ioClass/hd44780_pinIO.h>  // Arduino pin i/o class header
#include <OneWire.h>                       // One Wire library
#include <DallasTemperature.h>             // DS18B20 library

#define ONE_WIRE_BUS 2                   // DS18B20 data wire is connected to digital pin 2

DeviceAddress thermometerAddress;        // custom array type to hold 64 bit DS18B20 device address
OneWire oneWire(ONE_WIRE_BUS);           // create a oneWire instance to communicate with temperature IC
DallasTemperature tempSensor(&oneWire);  // pass the oneWire reference to Dallas Temperature

// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// declare object for GPIO expander LCD: auto locate & auto config expander chip
hd44780_I2Cexp lcd;

// declare Arduino pins used for LCD direct pin control
// this method does not control the backlight
// note that ESP8266 based arduinos must use the Dn defines rather than
// raw pin numbers.
#if defined (ARDUINO_ARCH_ESP8266)
const int rs = D8, en = D9, db4 = D4, db5 = D5, db6 = D6, db7 = D7; // for esp8266 devices
#else
const int rs = 8, en = 9, db4 = 4, db5 = 5, db6 = 6, db7 = 7;       // for all other devices
#endif
hd44780_pinIO lcdParallel(rs, en, db4, db5, db6, db7);

/*
  //with backlight control:
  //  backlight control requires two additional parameters
  //  - an additional pin to control the backlight
  //  - backlight active level which tells the library the level
  //    needed to turn on the backlight.
  //    note: If the backlight control pin supports PWM, dimming can be done
  //      using setBacklight(dimvalue);
  #if defined (ARDUINO_ARCH_ESP8266)
  const int rs = D8, en = D9, db4 = D4, db5 = D5, db6 = D6, db7 = D7, bl = D10, blLevel = HIGH;
  #else
  const int rs = 8, en = 9, db4 = 4, db5 = 5, db6 = 6, db7 = 7, bl = 10, blLevel = HIGH;
  #endif
  hd44780_pinIO lcd(rs, en, db4, db5, db6, db7, bl, blLEvel);
*/

void setup()
{
  Serial.begin(9600);

  // initialize I2C LCD with number of columns and rows:
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.lineWrap();  // turn on automatic line wrapping

  // initialize parallel LCD with number of columns and rows:
  lcdParallel.begin(LCD_COLS, LCD_ROWS);
  lcdParallel.lineWrap();  // turn on automatic line wrapping

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
  
  tempSensor.setResolution(thermometerAddress, 11);      // set the temperature resolution (9-12)
}

void loop() {

  tempSensor.requestTemperatures();                      // request temperature sample from sensor on the one wire bus
  displayTemp(tempSensor.getTempC(thermometerAddress));  // show temperature on LCD displays

  delay(500);                                            // update readings every half a second
}

// display temperature on LCDs
void displayTemp(float temperatureReading) {             // temperature comes in as a float with 2 decimal places
  Serial.print("Temperature: ");

  // show temperature °C
  lcdParallel.clear();
  lcdParallel.print("Temperature: ");
  lcdParallel.setCursor(0, 1);
  lcdParallel.print(temperatureReading, 1);  // rounded to 1 decimal place
  lcdParallel.print((char)223);              // degree symbol
  lcdParallel.print("C");

  Serial.print(temperatureReading);          // serial debug output
  Serial.print("°");
  Serial.print("C  ");

  // show temperature °F
  lcd.clear();
  lcd.print("Temperature: ");
  lcd.setCursor(0, 1);
  lcd.print(DallasTemperature::toFahrenheit(temperatureReading), 1);     // rounded to 1 decimal place
  lcd.print((char)223);                                                  // degree symbol
  lcd.print("F");

  Serial.print(DallasTemperature::toFahrenheit(temperatureReading));     // serial debug output
  Serial.print("°");
  Serial.print("F");

  Serial.println();
}

// print DS18B20 device address from the custom address array
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
