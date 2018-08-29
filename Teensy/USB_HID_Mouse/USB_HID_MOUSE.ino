
/*
  Original code has been modified for Teensy 3.6

  Based on the JoystickMouseControl example code by Tom Igoe
  http://www.arduino.cc/en/Tutorial/JoystickMouseControl
  
  This example code is in the public domain.

  For Teensy, choose USB Type Mouse (or a type that includes Mouse) in the Tools menu.

  This sketch enables the Teensy to appear as a USB HID mouse controller, using a 2 axis
  analog joystick and buttons to control the mouse.

  Hardware:
  - 2-axis joystick connected to pins A0 and A1
  - pushbuttons connected to digital pins 0 and 1

  The mouse movement is always relative. This sketch reads two analog inputs
  that range from 0 to 1023 (or less on either end) and translates them into
  ranges of -6 to 6.
  The sketch assumes that the joystick resting values are around the middle of
  the range, but that they vary within a threshold.
*/

// set pin numbers for switch, joystick axes, and LED:
const int mouseEnable = 1;        // switch to turn on and off mouse control
const int mouseLeftButton = 0;    // input pin for the mouse pushButton
const int xAxis = A0;             // joystick X axis
const int yAxis = A1;             // joystick Y axis
const int ledPin = 13;            // Mouse control LED

// parameters for reading the joystick:
int range = 12;               // output range of X or Y movement
int responseDelay = 5;        // response delay of the mouse, in ms
int threshold = range / 4;    // resting threshold
int center = range / 2;       // resting position value

bool mouseIsActive = false;   // whether or not to control the mouse
int lastSwitchState = LOW;    // previous switch state

void setup() {
  pinMode(mouseEnable, INPUT_PULLUP);           // the mouse enable toggle switch
  pinMode(mouseLeftButton, INPUT_PULLUP);       // the left mouse button pin
  pinMode(ledPin, OUTPUT);                      // the LED pin
  Serial.begin(9600);
  Mouse.begin();                                // take control of the mouse:
  Serial.println("USB HID Mouse Test.");        // debug message to confirm sketch is running
}

void loop() {
  // read the mouse enable switch to control if mouse is active or disabled
  int switchState = digitalRead(mouseEnable);
  if (switchState != lastSwitchState) {
    if (switchState == HIGH) {
      mouseIsActive = !mouseIsActive;
      digitalWrite(ledPin, mouseIsActive);     // turn on LED to indicate mouse state:
    }
  }
  // save switch state for next comparison:
  lastSwitchState = switchState;

  // read and scale the two axes:
  int xReading = readAxis(xAxis);
  int yReading = readAxis(yAxis);

  // if the mouse control state is active, move the mouse:
  if (mouseIsActive) {
    Mouse.move(xReading, yReading, 0);         //  not using scroll wheel, just move X and Y                
  }

  // read the left mouse button and assert or release a click if necessary:
  if (digitalRead(mouseLeftButton) == LOW) {   //  low means button is currently pressed
    if (!Mouse.isPressed(MOUSE_LEFT)) {        //  so assert the left mouse button if not already done
      Mouse.press(MOUSE_LEFT);
    }
  }
  // else the mouse button is currently high/not pressed:
  else {
    if (Mouse.isPressed(MOUSE_LEFT)) {         // if the left mouse button is currently pressed, release it:
      Mouse.release(MOUSE_LEFT);
    }
  }
  delay(responseDelay);
}

/*
  reads an axis (0 or 1 for x or y) and scales the analog input range to a range
  from 0 to <range>

 Analog Reading:    0 -----------------------------<Center>----------------------------- 1023
 Scaled Reading:    0 -----------------------------<Center>----------------------------- <range=12>
  
*/

int readAxis(int thisAxis) {
  // read the analog input:
  int reading = analogRead(thisAxis);

  // map the reading from the analog input range to the output range:
  reading = map(reading, 0, 1023, 0, range);

  // if the output reading is outside from the rest position threshold, use it:
  int distance = reading - center;

  if (abs(distance) < threshold) {
    distance = 0;
  }

  // return the distance for this axis:
  return distance;
}


