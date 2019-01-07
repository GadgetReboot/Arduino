// ----------------------------------------------------------------------------
// How to use the FastLED library for simple control of WS2812B RGB LEDs
// Quick and dirty test to turn one LED on fully white, all LEDs off,
// all LEDs on fully white, then repeat a fill pattern where all LEDs are
// turned on, then off, one by one.
//
// Used for current draw measurements to see how much current a strip needs.
//
// This example code is unlicensed and is released into the public domain
//
// Gadget Reboot
// ----------------------------------------------------------------------------


#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 60

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 2
#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  // Uncomment/edit one of the following lines for your leds arrangement.
  // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);

  // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<P9813, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<APA102, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<DOTSTAR, RGB>(leds, NUM_LEDS);

  // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);

}

void loop() {

  Serial.print("Brightness: ");
  Serial.println(255);
  FastLED.setBrightness( 255 );

  Serial.println("Testing one white LED at full brightness...");
  leds[0] = CRGB::White;
  FastLED.show();

  delay(5000);

  Serial.println("Testing all LEDs at zero brightness...");
  leds[0] = CRGB::Black;
  FastLED.show();

  delay(5000);

  Serial.println("Testing all white LEDs at full brightness...");
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed++) {
    leds[whiteLed] = CRGB::White;
  }
  FastLED.show();

  delay(5000);

  for (int whiteLed = (NUM_LEDS - 1); whiteLed >= 0; whiteLed--) {
    leds[whiteLed] = CRGB::Black;
  }
  FastLED.show();

  while (1) {
    for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed++) {
      // Turn the LED on, then pause
      //Serial.println("Red On");
      leds[whiteLed] = CRGB::White;
      FastLED.show();
      delay(20);
    }

    for (int whiteLed = (NUM_LEDS - 1); whiteLed >= 0; whiteLed--) {
      // Now turn the LED off, then pause
      // Serial.println("Red Off");
      leds[whiteLed] = CRGB::Black;
      FastLED.show();
      delay(20);
    }
  }

}
