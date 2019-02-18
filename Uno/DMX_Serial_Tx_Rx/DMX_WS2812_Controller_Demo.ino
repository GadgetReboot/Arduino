// DMX Controller Example for WS2812B RGB LEDs
// Target Hardware:  Arduino Uno
//
// Sends individual RGB levels for 60 WS2812B LEDs (60*3 = 180 DMX channels total)
// over an RS485 link in DMX packet format, to be decoded and sent out to a 60 LED strip.
// The test pattern is an LED chaser where two different color bars run end to end
// along the strip at different speeds, crossing over each other when they meet.
//
// The first DMX channel is 1 so the 60 LED RGB values are sent sequentially in
// channels 1 to 180, eg:
// Ch   1: LED  0 RED   value [0-255]
// Ch   2: LED  0 GREEN value [0-255]
// Ch   3: LED  0 BLUE  value [0-255]
// Ch 178: LED 59 RED   value [0-255]
// Ch 179: LED 59 GREEN value [0-255]
// Ch 180: LED 59 BLUE  value [0-255]
//
// Required library:
//    DMXSerial        install from library manager or https://github.com/mathertel/DMXSerial
//
// Gadget Reboot
// https://www.youtube.com/gadgetreboot


#include <DMXSerial.h>

#define dmxModePin            2    // RS485 data direction control pin
#define NUM_LEDS             60    // there will be RGB data for 60 LEDs

byte rgbData[NUM_LEDS * 3];        // array to hold RGB data for 60 LEDs

int onePos = 7;                    // position and direction markers for two LED chaser bars
int twoPos = 8;
byte oneDir = 0;
byte twoDir = 0;

void setup() {
  DMXSerial.init(DMXController, dmxModePin);   // initialize a DMX controller on the UART
}

void loop() {

  // draw the green and blue LED bars at their current
  // position along the LED strip by setting the relevant
  // color brightness levels in the LED RGB data array.
  // each bar is 3 LEDs long so set the color of an LED followed by
  // the led directly before and after it
  rgbData[onePos] = 150;
  rgbData[max(onePos - 3, 0)] = 150;
  rgbData[min(onePos + 3, (NUM_LEDS * 3) - 3)] = 150;

  rgbData[twoPos] = 150;
  rgbData[max(twoPos - 3, 0)] = 150;
  rgbData[min(twoPos + 3, (NUM_LEDS * 3) - 3)] = 150;

  // with the color bars configured in the LED RGB array,
  // send the entire array out as a DMX packet containing
  // 180 channels of RGB data for 60 LEDs (RGB x 60 = 180 ch)
  // delay a short while to create animated effect before updating
  // and re-drawing the next LED bars
  sendDmxData();
  delay(100);

  // turn recently drawn LEDs off (brightness = 0) so they can be
  // re-drawn on the next loop in the next calculated position.
  rgbData[onePos] = 0;
  rgbData[max(onePos - 3, 0)] = 0;
  rgbData[min(onePos + 3, (NUM_LEDS * 3) - 3)] = 0;

  rgbData[twoPos] = 0;
  rgbData[max(twoPos - 3, 0)] = 0;
  rgbData[min(twoPos + 3, (NUM_LEDS * 3) - 3)] = 0;

  // transmit the revised LED data
  sendDmxData();

  // calculate the next position of each LED line by advancing the
  // LED position in the forward or reverse direction as required.
  // if the line reaches the end of the LED strip, it is time to
  // change its direction.
  if (oneDir == 0) {     // if going forward
    onePos += 12;
    if (onePos >= (NUM_LEDS * 3)) {
      onePos = (NUM_LEDS * 3) - 5;
      oneDir = 1;  // go in reverse direction
    }
  }

  if (oneDir == 1) {     // if going reverse
    onePos -= 12;
    if (onePos <= 0) {
      onePos = 4;
      oneDir = 0;  // go in forward direction
    }
  }

  if (twoDir == 0) {     // if going forward
    twoPos += 9;
    if (twoPos >= (NUM_LEDS * 3)) {
      twoPos = (NUM_LEDS * 3) - 7;
      twoDir = 1;  // go in reverse direction
    }
  }

  if (twoDir == 1) {     // if going reverse
    twoPos -= 9;
    if (twoPos <= 0) {
      twoPos = 5;
      twoDir = 0;  // go in forward direction
    }
  }

}

// send the RGB LED data for 60 LEDs
// DMX channel 1 through 180 = LED array [0..179]
void sendDmxData() {
  for (int i = 1; i < (1 + (NUM_LEDS * 3)); i++) {
    DMXSerial.write(i, rgbData[i - 1]);
  }
}
