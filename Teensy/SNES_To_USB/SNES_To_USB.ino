/*
  SNES to USB Gamepad Controller
  Written for Teensy 3.6
  This code is in the public domain

  Controller Button States:
  snesRegister [11..0] = [RT, LT, X, A, Right, Left, Down, Up, Start, Select, Y, B]

  SNES controller pinout:
               ________________
              | 0 0 0 0 | 0 0 0 ) 
               ----------------      
                + C L D   N N G
                V L A A   C C N
                  K T T       D
                    C A
                    H

*/

// bit positions of each controller button in the status register
const int B_BUTTON               = 0;
const int Y_BUTTON               = 1;
const int SELECT_BUTTON          = 2;
const int START_BUTTON           = 3;
const int UP_BUTTON              = 4;
const int DOWN_BUTTON            = 5;
const int LEFT_BUTTON            = 6;
const int RIGHT_BUTTON           = 7;
const int A_BUTTON               = 8;
const int X_BUTTON               = 9;
const int LEFT_TRIGGER_BUTTON    = 10;
const int RIGHT_TRIGGER_BUTTON   = 11;

const int shiftDelay  = 20;  // delay in microseconds to help with shift register setup/hold timing

int snesRegister;            // SNES controller button states. 0=pressed 1=released

// SNES Pins
int snesClock      = 2;      // SNES controller clock pin
int snesLatch      = 3;      // SNES controller latch pin
int snesData       = 4;      // SNES controller data pin


void setup()
{
  // configure pins
  pinMode(snesData, INPUT);
  pinMode(snesClock, OUTPUT);
  pinMode(snesLatch, OUTPUT);

  digitalWrite(snesClock, LOW);      // SNES control lines idle low
  digitalWrite(snesLatch, LOW);

  Joystick.useManualSend(true);      // joystick data will be sent manually
}


void loop()
{
  // read the 12 controller buttons into the snesRegister
  // SNES controller button states are asynchronously loaded into the shift register while snesLatch is high
  // When snesLatch goes low, the first data bit is shifted to snesData
  // Button data is shifted to snesData on each low to high transition of snesClock
 
  digitalWrite(snesLatch, HIGH);       // while the latch pin is high, 
  delayMicroseconds(shiftDelay);       // impose a delay to allow a setup and hold time
  digitalWrite(snesLatch, LOW);  
        
  for (int x = 0; x <= 11; x++) {                    // read in the 12 controller buttons that were latched
    bitWrite(snesRegister, x, digitalRead(snesData)); // store the current button state on the data input into the correct snesRegister position
    digitalWrite(snesClock, HIGH);                   // generate a clock pulse to bring the next button to the data input
    delayMicroseconds(shiftDelay);
    digitalWrite(snesClock, LOW);                    
  }


  // assign the 12 SNES controller button states to the USB joystick buttons.
  // the USB button state logic is inverted so 1=pressed 0=released
  Joystick.button(1, !bitRead(snesRegister, B_BUTTON));
  Joystick.button(2, !bitRead(snesRegister, Y_BUTTON));
  Joystick.button(3, !bitRead(snesRegister, SELECT_BUTTON));
  Joystick.button(4, !bitRead(snesRegister, START_BUTTON));
  Joystick.button(5, !bitRead(snesRegister, UP_BUTTON));
  Joystick.button(6, !bitRead(snesRegister, DOWN_BUTTON));
  Joystick.button(7, !bitRead(snesRegister, LEFT_BUTTON));
  Joystick.button(8, !bitRead(snesRegister, RIGHT_BUTTON));
  Joystick.button(9, !bitRead(snesRegister, A_BUTTON));
  Joystick.button(10, !bitRead(snesRegister, X_BUTTON));
  Joystick.button(11, !bitRead(snesRegister, LEFT_TRIGGER_BUTTON));
  Joystick.button(12, !bitRead(snesRegister, RIGHT_TRIGGER_BUTTON));

  Joystick.send_now();      //send the current SNES controller button states to the USB joystick

}


