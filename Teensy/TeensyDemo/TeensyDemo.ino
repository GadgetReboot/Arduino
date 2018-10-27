/*********************************************************************
  This is an example for getting started using the Teensy 3.6
  in the Arduino development environment.

  This sketch uses code taken from an Arduino Uno sketch to control
  the 128x64 OLED display over I2C for seamless transfer to Teensy.

  PWM LED brightness control is implemented using PWM output,
  partially controlled by a potentiometer on an analog input to show
  some of the basic Arduino functionality being used on Teensy.

  This example requires the SSD1306 display driver by Adafruit:
  https://github.com/adafruit/Adafruit_SSD1306

*********************************************************************/

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// make sure the SSD1306 driver is configured for 64 line height
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int redPin        =  8;   // RGB LED pins
int greenPin      =  9;
int bluePin       = 10;

int potPin        = 13;   // potentiometer on analog 13
int potLevel      =  0;   // analog reading stored for potentiometer

byte ledIntensity =  0;   // potentiometer reading is used to set LED brightness
byte ledCounter   =  0;   // counter used to flash LED between two colors

void setup()   {

  Serial.begin(9600);

  // configure LED pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.println("Begin Teensy Demo.");

  // initialize OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
}


void loop() {

  potLevel = analogRead(potPin);             // read potentiometer
  ledIntensity = (potLevel / 4);       // scale pot reading down to a PWM 0-255 range

  // blue and red LED brightness are set by the pot, fading from opposite ends from full to dim
  analogWrite(bluePin, ledIntensity);        // set blue LED brightness with PWM
  analogWrite(redPin, 255 - ledIntensity);   // set red  LED brightness with PWM

  Serial.println();
  Serial.print("Pot: ");
  Serial.println(potLevel);
  Serial.print("Blue LED: ");
  Serial.println(ledIntensity);
  Serial.print("Red LED: ");
  Serial.println(255 - ledIntensity);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Pot:");
  display.println(potLevel);
  display.print("Blue: ");
  display.println(ledIntensity);
  display.print("Red: ");
  display.println(255 - ledIntensity);
  display.display();

  // as the LED counter increments, it is used to flash green LED.
  ledCounter++;
  if (ledCounter == 32) {
    ledCounter = 0;
  }
  Serial.print("LED Counter: ");
  Serial.println(ledCounter);

  // set LED pins
  if (ledCounter > 15) {
    digitalWrite(greenPin, LOW);
    Serial.println("Green: On");
  }
  else {
    digitalWrite(greenPin, HIGH);
    Serial.println("Green: Off");
  }




}







