// Temperature display using the DS18B20 digital thermometer
// and a 16x2 LCD display connected directly to Arduino (no serial interface)

#include <hd44780.h>                       // LCD hd44780 library
#include <hd44780ioClass/hd44780_pinIO.h>  // Arduino pin i/o class header
#include <OneWire.h>                       // One Wire library
#include <DallasTemperature.h>             // DS18B20 library

#define ONE_WIRE_BUS 2                   // DS18B20 data wire is connected to digital pin 2

OneWire oneWire(ONE_WIRE_BUS);           // create a oneWire instance to communicate with temperature IC
DallasTemperature tempSensor(&oneWire);  // pass the oneWire reference to Dallas Temperature

const int rs = 8, en = 9, db4 = 4, db5 = 5, db6 = 6, db7 = 7;   // LCD pins on Arduino
hd44780_pinIO lcd(rs, en, db4, db5, db6, db7);

void setup()
{
  lcd.begin(16, 2);      // initialize the 16x2 LCD
  tempSensor.begin();    // initialize the temp sensor
}

void loop() {
  // request temperature sample from sensor on the one wire bus
  tempSensor.requestTemperatures();
  float tempDegC = tempSensor.getTempCByIndex(0);
  float tempDegF = DallasTemperature::toFahrenheit(tempDegC);

  // show temperature °C
  lcd.clear();
  lcd.print("Temp: ");
  lcd.print(tempDegC, 1);  // rounded to 1 decimal place
  lcd.print((char)223);    // degree symbol
  lcd.print("C");

  // show temperature °F
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(tempDegF, 1);  // rounded to 1 decimal place
  lcd.print((char)223);    // degree symbol
  lcd.print("F");

  delay(1000);
}

