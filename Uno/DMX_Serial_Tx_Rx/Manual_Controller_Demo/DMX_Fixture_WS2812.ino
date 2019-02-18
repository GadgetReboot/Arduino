// DMX Receiver/Fixture Example
// Target Hardware:  Arduino Uno
//
// Receives DMX data for red, green, and blue color levels as
// well as a master brightness level, and controls an entire strip
// of 60 WS2812B RGB LEDs as a single light fixture all with the
// same color and brightness settings.
//
// Required library:
//    DMXSerial        install from library manager or https://github.com/mathertel/DMXSerial
//    ws2812.h         custom LED controller for better compatibility with DMX library
//
// Gadget Reboot
// https://www.youtube.com/gadgetreboot

#include <DMXSerial.h>
#include "ws2812.h"             // custom LED controller for compatibility
#define dmxModePin        2     // configures RS485 for receive mode
#define dmxBaseCh        10     // DMX channel 10 is the start channel. red = 10, grn = 11, blu = 12, bright = 14
#define NUM_LEDS         60     // 60 LEDs numbered [0..59]


byte maxBrightness     = 75;    // brightness range [off..on] = [0..255], keep dim for less current draw
byte redLevel,                  // store the received channel level control data
     grnLevel,
     bluLevel,
     brightness;
byte redCh             =   0;   // DMX channel offsets from base channel
byte grnCh             =   1;
byte bluCh             =   2;
byte brightnessCh      =   4;
byte ledRgbData[NUM_LEDS * 3];  // array to store RGB data for all 60 LEDs (180 bytes total)

void setup () {
  DMXSerial.init(DMXProbe, dmxModePin);   // initialize DMX receiver in manual mode
  DMXSerial.maxChannel(dmxBaseCh + 4);    // limit the number of DMX channels to read in

  setupNeopixel();                        // configure LED strip for pin 12 (hard coded in ws2812.h)
}


void loop() {

  // when a DMX packet has arrived, read in the channel data
  // and set the color levels in the LED array
  if (DMXSerial.receive()) {
    redLevel = DMXSerial.read(dmxBaseCh + redCh);
    grnLevel = DMXSerial.read(dmxBaseCh + grnCh);
    bluLevel = DMXSerial.read(dmxBaseCh + bluCh);

    brightness = DMXSerial.read(dmxBaseCh + brightnessCh);
    brightness = map(brightness, 1, 255, 0, maxBrightness);

    // scale the RGB color levels based on the brightness level requested
    redLevel = float(redLevel) * (float(brightness) / float(maxBrightness));
    grnLevel = float(grnLevel) * (float(brightness) / float(maxBrightness));
    bluLevel = float(bluLevel) * (float(brightness) / float(maxBrightness));

    // update all the red, green, and blue levels in the LED array
    // so that all reds are the same, all greens are the same, all blues are the same
    for (int i = 0; i < (NUM_LEDS * 3); i += 3) {
      ledRgbData[i] = redLevel;
    }

    for (int i = 1; i < (NUM_LEDS * 3); i += 3) {
      ledRgbData[i] = grnLevel;
    }

    for (int i = 2; i < (NUM_LEDS * 3); i += 3) {
      ledRgbData[i] = bluLevel;
    }
  }

  // update the led strip with the RGB data from the array
  updateNeopixel(ledRgbData, NUM_LEDS);
}
