/*
   Original sketch taken from https://github.com/PaulStoffregen/DmxSimple
   Modified by Gadget Reboot to use with a demo for two
   American DJ Micro Wash RGBW fixtures

    Welcome to DmxSimple. This library allows you to control DMX stage and
** architectural lighting and visual effects easily from Arduino. DmxSimple
** is compatible with the Tinker.it! DMX shield and all known DIY Arduino
** DMX control circuits.
**
** DmxSimple is available from: http://code.google.com/p/tinkerit/
** Help and support: http://groups.google.com/group/dmxsimple       */

#include <DmxSimple.h>

void setup() {

  /* DMX devices typically need to receive a complete set of channels
  ** even if you only need to adjust the first channel. You can
  ** easily change the number of channels sent here. If you don't
  ** do this, DmxSimple will set the maximum channel number to the
  ** highest channel you DmxSimple.write() to. */
  //DmxSimple.maxChannel(10);
  DmxSimple.usePin(3);
  DmxSimple.write(5, 255);     // set master brightness to max
  DmxSimple.write(10, 255);    // set master brightness to max
}

void loop() {
  int brightness;

  // RGBW Light Fixture 1

  for (brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(1, brightness);
    delay(5);
  }
  for (brightness = 255; brightness >= 0; brightness--) {
    DmxSimple.write(1, brightness);
    delay(5);
  }

  for (brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(2, brightness);
    delay(5);
  }
  for (brightness = 255; brightness >= 0; brightness--) {
    DmxSimple.write(2, brightness);
    delay(5);
  }

  for (brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(3, brightness);
    delay(5);
  }
  for (brightness = 255; brightness >= 0; brightness--) {
    DmxSimple.write(3, brightness);
    delay(5);
  }

  for (brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(4, brightness);
    delay(5);
  }
  for (brightness = 255; brightness >= 0; brightness--) {
    DmxSimple.write(4, brightness);
    delay(5);
  }


  // RGBW Light Fixture 2

  for (brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(6, brightness);
    delay(5);
  }
  for (brightness = 255; brightness >= 0; brightness--) {
    DmxSimple.write(6, brightness);
    delay(5);
  }

  for (brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(7, brightness);
    delay(5);
  }
  for (brightness = 255; brightness >= 0; brightness--) {
    DmxSimple.write(7, brightness);
    delay(5);
  }

  for (brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(8, brightness);
    delay(5);
  }
  for (brightness = 255; brightness >= 0; brightness--) {
    DmxSimple.write(8, brightness);
    delay(5);
  }

  for (brightness = 0; brightness <= 255; brightness++) {
    DmxSimple.write(9, brightness);
    delay(5);
  }
  for (brightness = 255; brightness >= 0; brightness--) {
    DmxSimple.write(9, brightness);
    delay(5);
  }

}
