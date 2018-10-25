/*******************************************************************************
       This is an example for using the X9Cxxx digital potentiometers
       in the feedback loop of an op amp to control the gain digitally.

       Uses the Bounce2 library for push button debouncing

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

#define UD              4       // pot up/down mode pin
#define INC             3       // pot increment pin
#define CS              2       // pot chip select pin

#define signalIn        A0      // op amp output pin to read in
#define debugIn         A1      // analog test input to compare op amp pre/post gain

#define buttonPotUp5    5       // button to inc pot by 10
#define buttonPotDn5    6       // button to dec pot by 10
#define buttonAutoScale 7       // button to auto-adjust pot until gain is max


// X9C wiring:  pin 3[High Terminal] -- R1 -- pin 5[Wiper] -- R2 -- pin 6[Low Terminal]

// The "X9C_UP" direction refers to the amount of resistance being created between the wiper and the "High" terminal
// rather than the position of the wiper itself moving up toward the high terminal
// (which would reduce resistance from wiper to High).
// i.e. setPot(70, false) will set the resistance between the X9C device pins 5 and 3 to 70% of maximum resistance
// where R1 = 70% of max, R2 = 30% of max

const int debounceInterval = 10;    // debounce time (ms) for button readings
bool  autoScale = true;             // toggle whether to allow serial plotter to autoscale or stay fixed

X9C pot;                            // instantiate a pot controller
Bounce buttonPotUp5DB = Bounce();   // instantiate a bounce object for each button
Bounce buttonPotDn5DB = Bounce();
Bounce buttonAutoScaleDB = Bounce();



void setup() {

  Serial.begin(9600);

  pot.begin(CS, INC, UD);
  pot.setPotMin(false);                // set pot Vw to Vh = 0% for minimum op amp gain to start (avoid saturation)

  pinMode(buttonPotUp5, INPUT_PULLUP);
  pinMode(buttonPotDn5, INPUT_PULLUP);
  pinMode(buttonAutoScale, INPUT_PULLUP);


  // attach buttons to debouncers
  buttonPotUp5DB.attach(buttonPotUp5);
  buttonPotUp5DB.interval(debounceInterval);      // interval in ms

  buttonPotDn5DB.attach(buttonPotDn5);
  buttonPotDn5DB.interval(debounceInterval);      // interval in ms

  buttonAutoScaleDB.attach(buttonAutoScale);
  buttonAutoScaleDB.interval(debounceInterval);   // interval in ms

}

void loop() {

  // update the Bounce instances
  buttonPotUp5DB.update();
  buttonPotDn5DB.update();
  buttonAutoScaleDB.update();

  // change potentiometer setting if required based on button presses,
  // storing the setting in the chip if "true" is passed to the pot controller

  if ( buttonPotUp5DB.fell()) {
    pot.trimPot(5, X9C_UP, false);      // move pot wiper 5 positions up
  }

  if ( buttonPotDn5DB.fell()) {
    pot.trimPot(5, X9C_DOWN, false);    // move pot wiper 5 positions down
  }

  /*
     This "feature" didn't work in the serial monitor on my system so I disabled it.
     A ghost plot was sticking in the background of the live data on the serial plotter
     so I assume it's a bug with the plotter (??) so I just leave the entire program
     fixed to allow autoscaling or keep it fixed the whole time.

    if ( buttonAutoScaleDB.fell()) {
      autoScale = !autoScale;            // toggle auto scaling of serial plotter
    }
  */
  if (!autoScale) {                      // plot upper and lower fixed lines if not autoscaling plotter

    Serial.print(880);
    Serial.print(" ");

    Serial.print(0);
    Serial.print(" ");
  }

  Serial.print(analogRead(signalIn));    // plot straight line to set a visual reference line
  Serial.print(" ");

  Serial.print(analogRead(debugIn));     // plot straight line to set a visual reference line
  Serial.print(" ");

  Serial.println();

}
