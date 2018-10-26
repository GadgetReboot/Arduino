/*******************************************************************************
       This is an example for using the X9Cxxx digital potentiometers
       in the feedback loop of an op amp to control the gain digitally.

       Push buttons allow manually setting the pot up or down in fixed increments
       as well as auto adjusting the gain to scale the input signal to "fit" within
       the analog input range.

       X9C wiring:  pin 3[High Terminal] -- R1 -- pin 5[Wiper] -- R2 -- pin 6[Low Terminal]

       The "X9C_UP" direction refers to the amount of resistance being created between the wiper and the "High" terminal
       rather than the position of the wiper itself moving up toward the high terminal
       (which would reduce resistance from wiper to High).
       i.e. setPot(70, false) will set the resistance between the X9C device pins 5 and 3 to 70% of maximum resistance
       where R1 = 70% of max, R2 = 30% of max

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


#include <X9C.h>                    // X9C pot library
#include <Bounce2.h>                // button debounce library

#define UD              4           // pot up/down mode pin
#define INC             3           // pot increment pin
#define CS              2           // pot chip select pin

#define signalIn        A0          // op amp output pin to read in
#define debugIn         A1          // analog test input to compare op amp pre/post gain

#define buttonPotUp5    5           // button to inc pot by 10
#define buttonPotDn5    6           // button to dec pot by 10
#define buttonFixedGain 7           // button to auto-adjust pot until gain matches a set point
#define buttonAutoGain  8           // button to auto-adjust pot until gain is scaled for sampled input max

#define debugMode       false       // set true or false when compiling to control verbose serial debug messages


const int debounceInterval        = 10;    // debounce time (ms) for button readings
const int fixedGainThreshold      = 600;   // target maximum input signal level (used to adjust gain to scale to)
const int inputSignalMaxThreshold = 740;   // maximum possible input signal level expected (from 0-1023) eg. 3.?v op amp max out
int       maxSignalLevel          = 0;     // target max analog input signal level to scale gain to
bool      autoScale               = true;  // toggle whether to allow serial plotter to autoscale or stay fixed
byte      potWiperPos             = 0;     // track the pot wiper position

X9C pot;                                   // instantiate a pot controller
Bounce buttonPotUp5DB = Bounce();          // instantiate a bounce object for each button
Bounce buttonPotDn5DB = Bounce();
Bounce buttonFixedGainDB = Bounce();
Bounce buttonAutoGainDB = Bounce();

void setup() {

  Serial.begin(9600);

  if (debugMode) Serial.println("Just started program  ");

  pot.begin(CS, INC, UD);
  pot.setPotMin(false);                   // set pot Vw to Vh = 0% for minimum op amp gain to start (avoid saturation)
  potWiperPos = 0;

  pinMode(buttonPotUp5, INPUT_PULLUP);
  pinMode(buttonPotDn5, INPUT_PULLUP);
  pinMode(buttonFixedGain, INPUT_PULLUP);
  pinMode(buttonAutoGain, INPUT_PULLUP);

  // attach buttons to debouncers
  buttonPotUp5DB.attach(buttonPotUp5);
  buttonPotUp5DB.interval(debounceInterval);      // interval in ms

  buttonPotDn5DB.attach(buttonPotDn5);
  buttonPotDn5DB.interval(debounceInterval);      // interval in ms

  buttonFixedGainDB.attach(buttonFixedGain);
  buttonFixedGainDB.interval(debounceInterval);   // interval in ms

  buttonAutoGainDB.attach(buttonAutoGain);
  buttonAutoGainDB.interval(debounceInterval);    // interval in ms
}

void loop() {

  // update the Bounce instances
  buttonPotUp5DB.update();
  buttonPotDn5DB.update();
  buttonFixedGainDB.update();
  buttonAutoGainDB.update();

  // change potentiometer setting if required based on button presses,
  // storing the setting in the chip if "true" is passed to the pot controller

  if ( buttonPotUp5DB.fell()) {
    if (debugMode) Serial.println("Pot up Button Pressed ");
    pot.trimPot(5, X9C_UP, false);      // move pot wiper 5 positions up
    potWiperPos += 5;
    if (potWiperPos > 99) potWiperPos = 99;            // limit max wiper count
    if (debugMode) Serial.print("Pot wiper position: ");
    if (debugMode) Serial.println(potWiperPos);
  }

  if ( buttonPotDn5DB.fell()) {
    if (debugMode) Serial.println("Pot Down Button Pressed ");
    pot.trimPot(5, X9C_DOWN, false);    // move pot wiper 5 positions down
    potWiperPos -= 5;
    if (potWiperPos < 0) potWiperPos = 0;            // limit min wiper count
    if (debugMode) Serial.print("Pot wiper position: ");
    if (debugMode) Serial.println(potWiperPos);
  }

  if ( buttonFixedGainDB.fell()) {
    if (debugMode) Serial.print("Fixed Gain Button Pressed ");
    if (debugMode) Serial.print("Pot wiper position: ");
    if (debugMode) Serial.println(potWiperPos);
    maxSignalLevel = fixedGainThreshold;
    setAutoGain();                      // adjust the op amp gain automatically
    if (debugMode) Serial.println("Fixed Gain has been adjusted ");
    if (debugMode) Serial.print("Pot wiper position: ");
    if (debugMode) Serial.println(potWiperPos);
  }

  if ( buttonAutoGainDB.fell()) {
    if (debugMode) Serial.print("Auto Gain Button Pressed ");
    if (debugMode) Serial.print("Pot wiper position: ");
    if (debugMode) Serial.println(potWiperPos);
    maxSignalLevel = inputSignalMaxThreshold;
    setAutoGain();                      // adjust the op amp gain automatically
    if (debugMode) Serial.println("Auto Gain has been adjusted ");
    if (debugMode) Serial.print("Pot wiper position: ");
    if (debugMode) Serial.println(potWiperPos);
  }

  if (!autoScale) {                     // plot upper and lower fixed lines if not autoscaling serial plotter
    Serial.print(880);
    Serial.print(" ");

    Serial.print(0);
    Serial.print(" ");
  }

  Serial.print(analogRead(signalIn));    // plot analog signal from op amp's output
  Serial.print(" ");

  Serial.print(analogRead(debugIn));     // plot original analog signal going into op amp
  Serial.print(" ");

  Serial.println();


}

// adjust the op amp gain pot until the signal reaches full scale based on the target upper analog
// threshold number.  
// max possible analog level is 1023 but some inputs may not be able to achieve this, eg an op amp 
// that can only put out 3.5 volts.  
// otherwise another arbitrary target max level may be used to set gain for a fixed max amplitude signal
// assumes a constant test signal is being applied to the analog signal input so multiple
// samples can be taken and the peaks detected.

void setAutoGain() {

  if (debugMode) Serial.println("Just Entered setAutoGain routine  ");

  bool gainSetComplete = false;
  bool initialGainTooHigh = false;                     // gain threshold checker

  if (getCalSampleMax() > maxSignalLevel) {            // determine if gain needs to be increased or decreased
    initialGainTooHigh = true;
  }

  if (initialGainTooHigh) {                            // sample gain is too high, try reducing
    while (!gainSetComplete) {
      if (potWiperPos > 0) {
        pot.trimPot(1, X9C_DOWN, false);               // move pot wiper 1 position down to reduce gain
        potWiperPos--;                                 // track pot wiper position
      }
      else {
        gainSetComplete = true;                        // gain is maxed out and pot can't be reduced further so quit
      }
      if (getCalSampleMax() <= maxSignalLevel) {       // check max level again, see if signal is reduced below threshold
        gainSetComplete = true;
      }
    }
  }
  
  else {                                               // sample gain is too low, try increasing
    while (!gainSetComplete) {
      if (potWiperPos < 99) {
        pot.trimPot(1, X9C_UP, false);                 // move pot wiper 1 position up
        potWiperPos++;                                 // track pot wiper position
      }
      else {
        gainSetComplete = true;                        // gain is maxed out and pot can't be increased further so quit
      }
      if (getCalSampleMax() > maxSignalLevel) {        // check max level again, see if signal is increased above threshold
        pot.trimPot(1, X9C_DOWN, false);               // move pot wiper 1 position down to reduce gain just below threshold
        potWiperPos--;                                 // track pot wiper position
        gainSetComplete = true;                        // gain has been adjusted, quit
      }
    }
  }
}    // end setAutoGain


int getCalSampleMax() {

  if (debugMode) Serial.println("Just entered getCalSampleMax routine  ");

  int sampleMax = 0;
  int currentSample;

  for (int i = 0; i <= 3500; i++) {                   //brute force sampling as a test. update to a timer later
    currentSample = analogRead(signalIn);

    if (currentSample > sampleMax) {
      sampleMax = currentSample;
    }
  }
  if (debugMode) Serial.print("Max sample level detected:  ");
  if (debugMode) Serial.println(sampleMax);
  return sampleMax;
}    // end getCalSampleMax








