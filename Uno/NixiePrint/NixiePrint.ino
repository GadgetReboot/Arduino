
// nixiePrint - takes a 4 digit integer and prints the
//              individual digits on 4 Nixie tubes with
//              a PCF8574 GPIO Expander
//
// Gadget Reboot, 2018

#include <Wire.h>

// PRINTBIN macro taken from a post by zhomeslice:  http://forum.arduino.cc/index.php?topic=475435.0
// prints a binary number on serial monitor with leading 0 padding
#define PRINTBIN(Num) for (uint32_t t = (1UL<< (sizeof(Num)*8)-1); t; t >>= 1) Serial.write(Num  & t ? '1' : '0'); // Prints a binary number with leading zeros (Automatic Handling)


int addr1 = 0x20;            // PCF8574 device 1
int addr2 = 0x21;            // PCF8574 device 2
int blank = 2;               // input switch to blank display when low

void setup() {
  Serial.begin(9600);
  pinMode(blank, INPUT_PULLUP);  // use digital pin 2 to GND to blank display

  // I2C init
  Wire.begin();

  // Turn off all Nixie displays by writing "1111" to all driver BCD inputs
  Wire.beginTransmission(addr1);
  Wire.write(0xFF);
  Wire.endTransmission();

  Wire.beginTransmission(addr2);
  Wire.write(0xFF);
  Wire.endTransmission();

  Serial.println();
  Serial.println("Nixie Display 4 Digit Counter");
  Serial.println();
}


void loop() {

  for (int i = 0; i < 9999; i++) {
    Serial.print("Counter: ");            // show the count on serial monitor in decimal
    Serial.println(i);
    nixiePrint(i, !digitalRead(blank));   // print the counter on the Nixie display, or blank display
    delay(200);
  }
}


//-------------------------------------------------------------------------
// nixiePrint - takes a 4 digit integer and separates out the digits
//              to send to Nixie tubes
//-------------------------------------------------------------------------
void nixiePrint(int number, bool blanking) {
  byte ones,         // separated out digits from input integer
       tens,
       hund,
       thou;
  byte lsb,          // output BCD data for four Nixie drivers
       msb;

  ones = number % 10;         // the result of (a number mod 10) is the least significant digit of the number.
  tens = (number / 10) % 10;  // then dividing by 10 will shift the decimal so the next
  hund = (number / 100) % 10; // digit can be found, etc.
  thou = (number / 1000) % 10;

  // blank leading zeroes for numbers by setting Nixie driver DCBA inputs to "1111"
  // eg. instead of displaying "0049", just display "49" with the left digits turned off
  if (number < 10) {        // if we only have one digit, blank the tens
    tens = 0xFF;
  }
  if (number < 100) {       // if we only have two digits or less, blank the hundreds
    hund = 0xFF;
  }
  if (number < 1000) {      // if we only have three digits or less, blank the thousands
    thou = 0xFF;
  }

  // combine the separate 4 bit digits into the two 8 bit LSB/MSB bytes to go to the GPIO expander
  // LSB:  7..4 = ones   3..0 = tens
  // MSB:  7..4 = hundreds   3..0 = thousands
  // to get the ones into the upper 4 bits of the byte, a left shift occurs 4 times to move the bits
  // then that is "or"ed with the tens, which is already in the lower 4 bit position so the final byte
  // has everything in the right place.  
  // the tens must be masked with 0x0F, where the "F" contains the valid 4 bit "tens" data and the "0" 
  // is forced into any bits that don't contain valid data. A "0" is required for the "or"ing to work   

  lsb = (ones << 4) | (0x0F & tens); // place "tens" in lower 4 bits and "ones" in upper 4 bits of LSB data
  msb = (hund << 4) | (0x0F & thou); // place "thou" in lower 4 bits and "hund" in upper 4 bits of MSB data

  // if the display is blanked, force all 1's to the Nixie control inputs
  // to disable all outputs on compatible BCD drivers.  If the driver does
  // not support blanking, the datasheet will detail the display behaviour
  // for an input of DCBA = 1111
  if (blanking == true) {
    lsb = 0xFF;
    msb = 0xFF;
    Serial.println("Nixie display is blanked...");
  }


  // show the count on serial monitor in binary to confirm DCBA data
  // show the exact DCBA data going out to Nixie tubes (counter, or blanking override)

  Serial.print("Ones: ");
  PRINTBIN(ones);
  Serial.print("  ");

  Serial.print("Tens: ");
  PRINTBIN(tens);
  Serial.print("  ");

  Serial.print("LSB: ");
  PRINTBIN(lsb);
  Serial.println();
  

  Serial.print("Hund: ");
  PRINTBIN(hund);
  Serial.print("  ");

  Serial.print("Thou: ");
  PRINTBIN(thou);
  Serial.print("  ");

  Serial.print("MSB: ");
  PRINTBIN(msb);
  Serial.println();

  Serial.println();


  // Send the LSB 8 bits out to "ones" and "tens" Nixie tubes on GPIO expander
  Wire.beginTransmission(addr1);
  Wire.write(lsb);
  Wire.endTransmission();

  // Send the MSB 8 bits out to "hund" and "thou" Nixie tubes on GPIO expander
  Wire.beginTransmission(addr2);
  Wire.write(msb);
  Wire.endTransmission();


}

