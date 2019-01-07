// ----------------------------------------------------------------------------
// How to use the FastLED library for simple control of WS2812B RGB LEDs
//
// This example code is unlicensed and is released into the public domain
//
// Gadget Reboot
// ----------------------------------------------------------------------------

#include <FastLED.h>

#define LED_TYPE    WS2812B   // type of RGB LEDs 
#define COLOR_ORDER GRB       // sequence of colors in data stream
#define NUM_LEDS    60        // 60 LEDs numbered [0..59]
#define DATA_PIN    2         // LED data pin
#define BRIGHTNESS  200       // brightness range [off..on] = [0..255]

// Define the array of RGB control data for each LED
CRGB leds[NUM_LEDS];

void setup() {

  // initialize library using CRGB LED array
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

}

void loop() {

  // configure some LEDs by direct RGB level setting
  leds[0] = CRGB(255, 0, 0);
  leds[1] = CRGB(0, 255, 0);
  leds[2] = CRGB(0, 0, 255);

  leds[3].r = 128;
  leds[3].g = 75;
  leds[3].b = 170;

  leds[4].setRGB(50, 100, 150);
  leds[5] = 0x08FFA7;

  // use a routine to fill a block of LEDs with a fixed color
  fill_solid( &(leds[6]), 5 /*number of leds*/, CRGB(100, 20, 130));

  // use a routine to fill a block of LEDs with a rainbow
  fill_rainbow( &(leds[11]), 10 /*number of leds*/, 1 /*starting hue*/);

  // configure some LEDs using hue, saturation, brightness
  leds[39] = CHSV( HUE_PURPLE, 255, 255);
  leds[38] = CHSV( HUE_PURPLE, 150, 255);
  leds[37] = CHSV( HUE_GREEN, 255, 255);
  leds[36] = CHSV( HUE_GREEN, 150, 255);
  leds[35] = CHSV( HUE_ORANGE, 255, 255);
  leds[34] = CHSV( HUE_ORANGE, 150, 255);

  // configure some LEDs using predefined RGB web color names
  leds[59] = CRGB::White;
  leds[58] = CRGB::HotPink;
  leds[57] = CRGB::DarkOrange;
  leds[56] = CRGB::Purple;
  leds[55] = CRGB::YellowGreen;
  leds[54] = CRGB::FireBrick;

  // optionally set the overall brightness for all LEDs
  FastLED.setBrightness(BRIGHTNESS);

  // activate all changes to LEDs
  FastLED.show();

  // wait a while, then gradually reduce brightness to 0
  delay(1000);

  FastLED.setBrightness(150);
  FastLED.show();
  delay(1000);

  FastLED.setBrightness(75);
  FastLED.show();
  delay(1000);

  FastLED.setBrightness(10);
  FastLED.show();
  delay(1000);

  FastLED.setBrightness(5);
  FastLED.show();
  delay(1000);

  FastLED.setBrightness(0);
  FastLED.show();
  delay(1000);

}
