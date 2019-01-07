// ----------------------------------------------------------------------------
// How to use the FastLED library for simple control of WS2812B RGB LEDs
// Two lines are drawn, one Green and one Blue, each 3 LEDs in length
// Each line moves end to end along the strip at slightly different rates
// If the pot is moved high enough from the ground terminal, the green
// line can be moved manually by the pot wiper position along the strip.
//
// This example code is unlicensed and is released into the public domain
//
// Gadget Reboot
// ----------------------------------------------------------------------------

#include <FastLED.h>

//#define DEBUG
#define NUM_LEDS 60
#define DATA_PIN 2
#define COLOR_ORDER GRB
#define LED_TYPE    WS2812B
#define POT A0

// Define the array of leds
CRGB leds[NUM_LEDS];

int onePos = 2;
int twoPos = 2;
byte oneDir = 0;
byte twoDir = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

}

void loop() {

  // if pot is above ground, override green line position based on pot position
  if (analogRead(POT) > 10) {
    onePos = map(analogRead(POT), 0, 1023, 0, NUM_LEDS - 1);
  }

#ifdef DEBUG
  Serial.print("Potentiometer: ");
  Serial.println(analogRead(POT));
#endif

  // turn on the green and blue LEDs to draw the current
  // position of each line along the strip
  leds[onePos] = CRGB(0, 255, 0);
  leds[max(onePos - 1, 0)] = CRGB(0, 255, 0);
  leds[min(onePos + 1, NUM_LEDS - 1)] = CRGB(0, 255, 0);

  leds[twoPos] = CRGB(0, 0, 255);
  leds[max(twoPos - 1, 0)] = CRGB(0, 0, 255);
  leds[min(twoPos + 1, NUM_LEDS - 1)] = CRGB(0, 0, 255);

  FastLED.show();
  delay(150);

  // turn all LEDs off so they can be re-drawn on the next loop
  // in the next calculated position
  leds[onePos] = CRGB(0, 0, 0);
  leds[max(onePos - 1, 0)] = CRGB(0, 0, 0);
  leds[min(onePos + 1, NUM_LEDS - 1)] = CRGB(0, 0, 0);

  leds[twoPos] = CRGB(0, 0, 0);
  leds[max(twoPos - 1, 0)] = CRGB(0, 0, 0);
  leds[min(twoPos + 1, NUM_LEDS - 1)] = CRGB(0, 0, 0);

  FastLED.show();

#ifdef DEBUG
  Serial.print("One Direction: ");
  Serial.println(oneDir);
  Serial.print("One Position: ");
  Serial.println(onePos);
  //Serial.println();

  Serial.print("Two Direction: ");
  Serial.println(twoDir);
  Serial.print("Two Position: ");
  Serial.println(twoPos);
  Serial.println();
#endif

  // calculate the next position of each LED line by advancing the
  // LED position in the forward or reverse direction as required.
  // if the line reaches the end of the LED strip, it is time to
  // change its direction.
  if (oneDir == 0) {     // if going forward
    onePos += 4;
    if (onePos >= NUM_LEDS) {
      onePos = NUM_LEDS - 1;
      oneDir = 1;  // go in reverse direction
    }
  }
  // else {   //if going reverse
  if (oneDir == 1) {
    onePos -= 4;
    if (onePos <= 0) {
      onePos = 0;
      oneDir = 0;  // go in forward direction
    }
  }

  if (twoDir == 0) {     // if going forward
    twoPos += 3;
    if (twoPos >= NUM_LEDS) {
      twoPos = NUM_LEDS - 1;
      twoDir = 1;  // go in reverse direction
    }
  }
  //else {   //if going reverse
  if (twoDir == 1) {
    twoPos -= 3;
    if (twoPos <= 0) {
      twoPos = 0;
      twoDir = 0;  // go in forward direction
    }
  }

}
