// I2C Version

#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

void setup()
{
  lcd.begin(16, 2);
  lcd.print("Hello, World!");
}

void loop() {}
