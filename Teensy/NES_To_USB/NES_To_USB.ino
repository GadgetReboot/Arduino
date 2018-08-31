/*
  NES to USB Gamepad Controller
  Written for Teensy 3.6
  This code is in the public domain

  Controller Button States:
  nesRegister [7..0] = [Right, Left, Down, Up, Start, Select, B, A]

  NES controller pinout:
               _________
             /          |
            /        O  | Ground
           /            |
     +VCC  |  O      O  | Clock
           |            |
     N.C.  |  O      O  | Latch
           |            |
     N.C.  |  O      O  | Data Out (To NES)
           |____________|

*/


// bit positions of each controller button in the status register
const int A_BUTTON         = 0;
const int B_BUTTON         = 1;
const int SELECT_BUTTON    = 2;
const int START_BUTTON     = 3;
const int UP_BUTTON        = 4;
const int DOWN_BUTTON      = 5;
const int LEFT_BUTTON      = 6;
const int RIGHT_BUTTON     = 7;

const int shiftDelay       = 20;    // delay in microseconds to help with shift register setup/hold timing

byte nesRegister           = 255;   // NES controller button states. 0=pressed 1=released

// NES Pins
int nesClock      = 2;    // NES controller 4021 clock pin
int nesLatch      = 3;    // NES controller 4021 latch pin
int nesData       = 4;    // NES controller 4021 data pin


void setup()
{
  // configure pins
  pinMode(nesData, INPUT);
  pinMode(nesClock, OUTPUT);
  pinMode(nesLatch, OUTPUT);

  digitalWrite(nesClock, LOW);      // NES control lines idle low
  digitalWrite(nesLatch, LOW);

  Joystick.useManualSend(true);     // analog joystick data will be sent manually
}


void loop()
{
  // read the 8 controller buttons into the nesRegister
  // NES controller button states are asynchronously loaded into the 4021 while nesLatch is high
  // When nesLatch goes low, the first data bit is shifted to nesData
  // Button data is shifted to nesData on each low to high transition of nesClock
 
  digitalWrite(nesLatch, HIGH);       // while the latch pin is high, buttons 
  delayMicroseconds(shiftDelay);      // impose a delay to allow a setup and hold time for any 4021 pins
  digitalWrite(nesLatch, LOW);  
        
  for (int x = 0; x <= 7; x++) {                    // read in the 8 controller buttons that were latched into the 4021
    bitWrite(nesRegister, x, digitalRead(nesData)); // store the current button state on the data input into the correct nesRegister position
    digitalWrite(nesClock, HIGH);                   // generate a clock pulse to bring the next button to the data input
    delayMicroseconds(shiftDelay);
    digitalWrite(nesClock, LOW);                    
  }


  // assign the 8 NES controller button states to the USB joystick buttons.
  // the USB button state logic is inverted so 1=pressed 0=released
  Joystick.button(1, !bitRead(nesRegister, A_BUTTON));
  Joystick.button(2, !bitRead(nesRegister, B_BUTTON));
  Joystick.button(3, !bitRead(nesRegister, SELECT_BUTTON));
  Joystick.button(4, !bitRead(nesRegister, START_BUTTON));
  Joystick.button(5, !bitRead(nesRegister, UP_BUTTON));
  Joystick.button(6, !bitRead(nesRegister, DOWN_BUTTON));
  Joystick.button(7, !bitRead(nesRegister, LEFT_BUTTON));
  Joystick.button(8, !bitRead(nesRegister, RIGHT_BUTTON));

  //Joystick.X(analogRead(0));      //read analog joystick X and Y positions. Commented out for pure NES functionality
  //Joystick.Y(analogRead(1));

  Joystick.send_now();      //send the current NES controller button states to the USB joystick

}


