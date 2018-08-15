
// PCF8574 Multiple I2C device access demo
// Gadget Reboot, 2018

#include <Wire.h>

int addr1 = 0x20;          // PCF8574 device 1
int addr2 = 0x21;          // PCF8574 device 2
int LED_Pattern = 0xFFFE;  // LED Sequence Start pattern, one LED on
bool ISR_FLAG = false;     // Interrupt triggered flag


void setup() {
  
  // I2C init
  Wire.begin();            
  
  // Turn off all GPIO pins on both I2C expanders by writing all "1"
  Wire.beginTransmission(addr1);
  Wire.write(0xFF);
  Wire.endTransmission();

  Wire.beginTransmission(addr2);
  Wire.write(0xFF);
  Wire.endTransmission();

  // Assign an interrupt routine using Uno pin 2 
  // PCF8574 /int output connects to Uno pin 2 
  // Interrupt is triggered when a GPIO input changes from high to low
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), ISR_PROC, FALLING);

}


void loop() {
  LED_Pattern = 0xFFFE;                   // Start each LED chase cycle with one LED on (set to "0") in the LSB position

  for (int i = 0; i < 16; i++)            // Cycle through all 16 GPIO pins, shifting the "ON" LED along
  {
    // Poll I2C device 2 to check if any input has been asserted low (button press)
    // If any button has been pressed, overwrite the LED pattern with alternating ON/OFF status
    // as long as any button on device 2 is being pressed.
    // When the button is released, normal LED sequencing will resume as the temporary 
    // pattern is flushed (shifted out of the display)
    
    ConfigureInputs(addr2);               // Configure device 2 for input polling (device 1 is interrupt driven)
    int InputData2 = ReadInputs(addr2);   // Read all 8 GPIO inputs on device 2
    if ( InputData2 != 0xFF)              // If any inputs were "0", a button has been pressed
    {
      LED_Pattern = 0x7E7E;               // To indicate a button was pressed, turn on LSB/MSB LED
    }

    // Send the LSB 8 bits of LED_Pattern out to device 1
    Wire.beginTransmission(addr1);
    Wire.write(lowByte(LED_Pattern));
    Wire.endTransmission();

    // Send the MSB 8 bits of LED_Pattern out to device 2
    Wire.beginTransmission(addr2);
    Wire.write(highByte(LED_Pattern));
    Wire.endTransmission();

    LED_Pattern = LED_Pattern << 1;  // Shift all bits left, moving the "0" along to turn on the next LED
    bitSet (LED_Pattern, 0);         // Set the LSB to "1" after shifting left so a "1" is piped in to keep trailing LEDs off
                                     // since shifting left will leave a "0" to the right, which will turn on an LED
    
    delay(100);                      // LED chase display pause

    // If an interrupt was flagged, force a unique pattern on the LEDs
    // and pause long enough to see it before resuming program
    // Something more useful can be done such as debouncing and 
    // detecting which input was triggered
    if (ISR_FLAG)   
    {
      Wire.beginTransmission(addr1);
      Wire.write(0x77);
      Wire.endTransmission();
      delay(2000);
      ISR_FLAG = false;              // Clear interrupt flag
    }
  }
}


//  Set a flag if an interrupt is triggered
void ISR_PROC()
{
  ISR_FLAG = true;
}

// Configure ports for input mode by writing all "1"'s to addressed device port
void ConfigureInputs(int address)
{
  Wire.beginTransmission(address);
  Wire.write(0xFF);                 // set ports high for input mode
  Wire.endTransmission();
}

// Read the port data on the addressed device port
byte ReadInputs(int address)
{
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 1);
  int Data_In = Wire.read();
  Wire.endTransmission();
  return Data_In;
}


// END PROGRAM

