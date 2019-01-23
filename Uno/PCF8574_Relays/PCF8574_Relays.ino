/************************************************************************************

   Using an Arduino Uno or an ESP8266 on NodeMCU to control buttons
   and relays through a PCF8574 GPIO expander.

   Requires the PCF8574 library
   https://github.com/RobTillaart/Arduino/tree/master/libraries/PCF8574


   If using Uno,     comment out #define intPin D4
   If using ESP8266, comment out #define intPin 2

   GPIO expander pinout:   0..3 = button input 0..3
                           4..7 = relay output 0..3
   Each button controls a corresponding relay 
   eg. button 0 controls relay 0

   Button functions:   button 0 and 1 toggle relay 0 and 1 on each press
                       button 2 and 3 directly control relay 2 and 3 in "realtime"


   Gadget Reboot

 *************************************************************************************/


#include <PCF8574.h>
#include <Wire.h>

PCF8574 pcf8574(0x20);

// uncomment the applicable line if using Uno or ESP8266
#define intPin 2                       // interrupt input Uno
//#define intPin D4                    // interrupt input NodeMCU ESP8266

volatile bool buttonPressed = false;   // button interrupt flag
byte buttonReg;                        // button read register

void setup() {

  // initialize PCF8574 with an interrupt pin and set all outputs to '1' (Relays off)
  Wire.begin();
  pinMode(intPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intPin), buttonISR, FALLING);
  pcf8574.begin(0xFF); // turn off all Relays

}

void loop() {

  // if a button press was detected via interrupt, check if the button press
  // occurred on one of the "toggle" control buttons and toggle the relay if so
  if (buttonPressed) {
    delay(50);  // crude debounce
    buttonReg = pcf8574.read8();

    // toggle a relay if button pressed
    if (!bitRead(buttonReg, 0)) {
      pcf8574.toggle(4);
    }

    // toggle a relay if button pressed
    if (!bitRead(buttonReg, 1)) {
      pcf8574.toggle(5);
    }
  }

  // clear interrupt flag
  buttonPressed = false;

  // manually poll last two buttons and set relays to match button on/off realtime state
  // method 1:  read all buttons and choose the desired button from the read register
  buttonReg = pcf8574.read8();
  pcf8574.write(6, bitRead(buttonReg, 2));

  // method 2:  directly read just one button and directly set the target relay
  pcf8574.write(7, pcf8574.read(3));

}

// interrupt service routine
void buttonISR() {
  buttonPressed = true;
}
