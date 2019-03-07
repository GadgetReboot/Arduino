// LS-30 Rotary Joystick to MAME Interface
// Reads the 12 rotary switches and detects when the position has changed.
// Generates a momentary output pulse to indicate a clockwise or
// counter-clockwise movement has occurred.
//
// MAME is configured to take CW/CCW inputs to play arcade games that used
// these joysticks with 12 inputs so this adapter is required to translate
// the joystick switches into usable rotation signals for MAME.
//
// Target Hardware:  Arduino Uno
//                   Rotary Joystick with 12 position switch and common connection
//
// Connections:
// Joystick Harness Pin     Arduino Pin
// 1, 5, 9                      2
// 2, 6, 10                     3
// 3, 7, 11                     4
// 4, 8, 12                     5
// 13                          GND
//
// Outputs:
// Clockwise                    8
// Counter-Clockwise            9
//
// Required libraries and board files:
// Bounce2               switch debouncer                 https://github.com/thomasfredericks/Bounce2
//
// Gadget Reboot
// https://www.youtube.com/gadgetreboot


#define DEBUG 0

#include <Bounce2.h>

#define debounceTime 40                                     // switch debounce in ms
#define cwOut         8                                     // cw  output pin
#define ccwOut        9                                     // ccw output pin
#define numInputs     4                                     // use 4 inputs to read the rotary switches
const uint8_t rotarySwitch[numInputs] = {2, 3, 4, 5};       // digital inputs for rotary switches

byte lastState = 0;      // previous stored joystick reading
byte curState  = 0;      // current joystick reading to evaluate against last reading
bool cw        = false;  // detected a clockwise rotation
bool ccw       = false;  // detected a counter-clockwise rotation
bool outputOn  = false;  // a cw or ccw output is currently being asserted (controlled by a timer)
unsigned long outputOnTime = 5;  // duration to assert outputs, in mS.
unsigned long outputTimer  = 0;  // timer for asserting outputs for required time lapse

#if Debug
byte debugCount = 0;     // for debug purposes, how many rotations detected since power on?
#endif

Bounce * buttons = new Bounce[numInputs];

void setup() {

#if Debug
  Serial.begin(9600);
  Serial.println("\nStart...\n");
#endif

  // outputs must idle high and assert active low
  // configure cw/ccw outputs by first setting them high,
  // then set them as outputs so they will be guaranteed high
  digitalWrite(cwOut, 1);
  digitalWrite(ccwOut, 1);
  pinMode(cwOut, OUTPUT);
  pinMode(ccwOut, OUTPUT);

  for (int i = 0; i < numInputs; i++) {
    buttons[i].attach( rotarySwitch[i] , INPUT_PULLUP  ); // setup the bounce instance for the current button
    buttons[i].interval(debounceTime);                    // debounce interval in ms
  }

  // take an initial reading of the rotary switches
  for (int i = 0; i < numInputs; i++)  {
    buttons[i].update();                                  // update the switch status
    if (buttons[i].read() == LOW) {                       // if a switch was grounded, update the status register
      bitSet(curState, i);
    }
  }

  lastState = curState;  // make last and current readings identical to avoid a false output trigger on power up

#if Debug
  Serial.print("Last Reading:  ");
  Serial.println(lastState, BIN);
  Serial.print("Cur  Reading:  ");
  Serial.println(curState, BIN);
#endif

}  // end setup()


void loop() {

  readJoystick();        // read the current rotary switch states into curState register
  processJoystick();     // determine if a rotation has occurred and set a flag if so

  if (cw | ccw) {        // if a rotation has been flagged, generate required output pulse
    generateOutput();
  }

  // if an output is being asserted and the timer has lapsed,
  // stop asserting the output
  if ( (outputOn) && (millis() > outputTimer + outputOnTime) ) {
    cancelOutput();
  }
}  // end loop()


// read the joystick rotary switches into the curState register
void readJoystick() {

  for (int i = 0; i < numInputs; i++)  {
    buttons[i].update();                   // update the debouncer status
    if (buttons[i].fell()) {               // if a switch was grounded, update the status register
      curState = 0;                        // clear the register and re-build it from current switch reading
      bitSet(curState, i);
    }
  }
}  // end readJoystick()


// check if rotary switches are different from last saved reading
// and set the appropriate flag for the detected rotation
void processJoystick() {

  int diff = (lastState - curState);  // check for a difference in joystick readings

  // clockwise rotation has occurred
  if ( ((diff < 0) && !((lastState == B0001) && (curState == B1000))) |
       ((diff > 0) && (lastState == B1000) && (curState == B0001)) ) {
#if Debug
    Serial.println("-----");
    Serial.println("CW Detected - Process Joystick");
    Serial.print("Last Reading:  ");
    Serial.println(lastState, BIN);
    Serial.print("Cur  Reading:  ");
    Serial.println(curState, BIN);
    Serial.println();
#endif
    cw = true;
  }

  // counter-clockwise rotation has occurred
  if ( ((diff > 0) && !((lastState == B1000) && (curState == B0001))) |
       ((diff < 0) && (lastState == B0001) && (curState == B1000)) ) {
#if Debug
    Serial.println("-----");
    Serial.println("CCW Detected - Process Joystick");
    Serial.print("Last Reading:  ");
    Serial.println(lastState, BIN);
    Serial.print("Cur  Reading:  ");
    Serial.println(curState, BIN);
    Serial.println();
#endif
    ccw = true;
  }

  // update the last joystick reading if there's a new position
  if (cw | ccw)
    lastState = curState;

}  // end processJoystick()


// assert the cw or ccw output signals low to indicate the direction of rotation.
void generateOutput() {

  if (cw) {
#if Debug
    Serial.println("Generating CW Out");
#endif
    digitalWrite(cwOut, 0);
    cw = false;
  }
  
  if (ccw) {
#if Debug
    Serial.println("Generating CCW Out");
#endif
    digitalWrite(ccwOut, 0);
    ccw = false;
  }
  outputOn = true;
  outputTimer = millis();   // reset timer for asserted output signal
}  // end generateOutput()


// cancel active output flag and stop asserting outputs, letting them idle high
void cancelOutput() {
  
#if Debug
  debugCount += 1;
  Serial.println("Clearing Outputs");
  Serial.print("Count: ");
  Serial.println(debugCount);
  Serial.println("-----");
#endif

  digitalWrite(cwOut, 1);
  digitalWrite(ccwOut, 1);
  outputOn = false;
}  // end cancelOutput()
