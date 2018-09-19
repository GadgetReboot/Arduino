/*******************************************************************************
       This is an example for using the X9Cxxx digital potentiometers,
       allowing control over the wiper position from min to max, and
       optionally saving the wiper position so it can be recalled
       automatically upon next power up.

       Uses the Bounce2 library (installed through Arduino library manager)

       Requires the X9C potentiometer control library by Phil Bowles
       https://github.com/philbowles/Arduino-X9C
       
       Note:  Changes required to the original X9C.cpp file from GitHub
       (as of Last commit Jun 8 2017)
       The following 2 functions should be replaced with this code:
       
       void X9C::_deselectAndSave(){

        digitalWrite(_inc,LOW);           //***********GADGET REBOOT CHANGED******************
        delayMicroseconds(1);             //***********GADGET REBOOT CHANGED******************
        digitalWrite(_inc,HIGH);          //***********GADGET REBOOT CHANGED******************
        delayMicroseconds(1);             //***********GADGET REBOOT CHANGED******************
        digitalWrite(_cs,HIGH);             // unselect chip and write current value to NVRAM
       }

      void X9C::_stepPot(uint8_t amt,uint8_t dir){
        uint8_t cnt=(amt > X9C_MAX) ? X9C_MAX:amt-1;  //***********GADGET REBOOT CHANGED******************
        digitalWrite(_ud,dir);              // set direction
        digitalWrite(_cs,LOW);              // select chip
        delayMicroseconds(1);
        while(cnt--){
          digitalWrite(_inc,LOW);           // falling pulse triggers wiper change (xN = cnt)
          delayMicroseconds(1);       
          digitalWrite(_inc,HIGH);
          delayMicroseconds(1);
          }
          delayMicroseconds(100);             // let new value settle; (datasheet P7 tIW)
        }


       This code is released to the public domain.
       Gadget Reboot

*******************************************************************************/


#include <X9C.h>                // X9C pot library
#include <Bounce2.h>            // button debounce library

#define UD              10      // pot up/down mode pin
#define INC             11      // pot increment pin
#define CS              12      // pot chip select pin
#define buttonPotMin    3       // button to set pot to min point
#define buttonPotMid    4       // button to set pot to mid point
#define buttonPotMax    5       // button to set pot to max point
#define buttonPotUp10   6       // button to inc pot by 10
#define buttonPotDn10   7       // button to dec pot by 10
#define buttonPotUp1    8       // button to inc pot by 1
#define buttonPotDn1    9       // button to dec pot by 1

// X9C wiring:  pin 3[High Terminal] -- R1 -- pin 5[Wiper] -- R2 -- pin 6[Low Terminal]

// The "X9C_UP" direction refers to the amount of resistance being created between the wiper and the "High" terminal 
// rather than the position of the wiper itself moving up toward the high terminal 
// (which would reduce resistance from wiper to High).
// i.e. setPot(70, false) will set the resistance between the X9C device pins 5 and 3 to 70% of maximum resistance
// where R1 = 70% of max, R2 = 30% of max

const int debounceInterval = 10;    // debounce time (ms) for button readings

X9C pot;                            // instantiate a pot controller
Bounce buttonPotMinDB  = Bounce();  // instantiate a bounce object for each button
Bounce buttonPotMidDB  = Bounce();
Bounce buttonPotMaxDB  = Bounce();
Bounce buttonPotUp10DB = Bounce();
Bounce buttonPotDn10DB = Bounce();
Bounce buttonPotUp1DB  = Bounce();
Bounce buttonPotDn1DB  = Bounce();


void setup() {

  pot.begin(CS, INC, UD);

  pinMode(buttonPotMin,  INPUT_PULLUP);
  pinMode(buttonPotMid,  INPUT_PULLUP);
  pinMode(buttonPotMax,  INPUT_PULLUP);
  pinMode(buttonPotUp10, INPUT_PULLUP);
  pinMode(buttonPotDn10, INPUT_PULLUP);
  pinMode(buttonPotUp1,  INPUT_PULLUP);
  pinMode(buttonPotDn1,  INPUT_PULLUP);

  // attach buttons to debouncers
  buttonPotMinDB.attach(buttonPotMin);
  buttonPotMinDB.interval(debounceInterval);      // interval in ms

  buttonPotMidDB.attach(buttonPotMid);
  buttonPotMidDB.interval(debounceInterval);      // interval in ms

  buttonPotMaxDB.attach(buttonPotMax);
  buttonPotMaxDB.interval(debounceInterval);      // interval in ms

  buttonPotUp10DB.attach(buttonPotUp10);
  buttonPotUp10DB.interval(debounceInterval);      // interval in ms

  buttonPotDn10DB.attach(buttonPotDn10);
  buttonPotDn10DB.interval(debounceInterval);      // interval in ms

  buttonPotUp1DB.attach(buttonPotUp1);
  buttonPotUp1DB.interval(debounceInterval);      // interval in ms

  buttonPotDn1DB.attach(buttonPotDn1);
  buttonPotDn1DB.interval(debounceInterval);      // interval in ms

}

void loop() {

  // update the Bounce instances
  buttonPotMinDB.update();
  buttonPotMidDB.update();
  buttonPotMaxDB.update();
  buttonPotUp10DB.update();
  buttonPotDn10DB.update();
  buttonPotUp1DB.update();
  buttonPotDn1DB.update();

  // change potentiometer setting if required based on button presses,
  // storing the setting in the chip if "true" is passed to the pot controller

  if ( buttonPotMinDB.fell()) {
    pot.setPotMin(false);                // set pot wiper to 0 resistance (pot will have wiper resistance per datasheet)
  }

  if ( buttonPotMidDB.fell()) {
    pot.setPot(49, false);               // set pot wiper to about half way (between 0 and 99)
  }

  if ( buttonPotMaxDB.fell()) {
    pot.setPotMax(false);                // set pot wiper to full resistance
  }

  if ( buttonPotUp10DB.fell()) {
    pot.trimPot(10, X9C_UP, true);      // move pot wiper 10 positions up ***AND STORE SETTING IN X9C CHIP***
  }

  if ( buttonPotDn10DB.fell()) {
    pot.trimPot(10, X9C_DOWN, false);    // move pot wiper 10 positions down
  }

  if ( buttonPotUp1DB.fell()) {
    pot.trimPot(1, X9C_UP, false);       // move pot wiper 1 position up
  }

  if ( buttonPotDn1DB.fell()) {
    pot.trimPot(1, X9C_DOWN, false);     // move pot wiper 1 position down
  }

}
