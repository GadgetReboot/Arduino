// Direct Pin Version

#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h> // Arduino pin i/o class header

const int rs=8, en=9, db4=4, db5=5, db6=6, db7=7;       

hd44780_pinIO lcd(rs, en, db4, db5, db6, db7);  // declare lcd object for pin control of LCD

void setup()
{
  lcd.begin(16, 2);
  lcd.print("Hello, World!");
  lcd.print(char(226));
}

void loop() {}
