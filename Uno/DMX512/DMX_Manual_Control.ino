/*
   Original sketch taken from https://github.com/PaulStoffregen/DmxSimple
   Modified by Gadget Reboot to use with a demo for two
   American DJ Micro Wash RGBW fixtures

** This program allows you to set DMX channels over the serial port.
**
** After uploading to Arduino, switch to Serial Monitor and set the baud rate
** to 9600. You can then set DMX channels using these commands:
**
** <number>c : Select DMX channel
** <number>v : Set DMX channel to new value
**
** These can be combined. For example:
** 100c355v : Set channel 100 to value 255.
**
** For more details, and compatible Processing sketch,
** visit http://code.google.com/p/tinkerit/wiki/SerialToDmx
**
** Help and support: http://groups.google.com/group/dmxsimple       */

#include <DmxSimple.h>

void setup() {
  Serial.begin(9600);

  /* DMX devices typically need to receive a complete set of channels
  ** even if you only need to adjust the first channel. You can
  ** easily change the number of channels sent here. If you don't
  ** do this, DmxSimple will set the maximum channel number to the
  ** highest channel you DmxSimple.write() to. */
  //DmxSimple.maxChannel(10);
  DmxSimple.usePin(3);   // digital output for DMX serial data
  DmxSimple.write(5, 255);     // set fixture #1 master brightness to max
  DmxSimple.write(10, 255);    // set fixture #2 master brightness to max

  Serial.println("DMX Manual Control");
  Serial.println();
  Serial.println("Syntax:");
  Serial.println(" 123c : use DMX channel 123 (range: 1-512)");
  Serial.println(" 45v  : set current channel to value 45 (range: 0-255)");
}

int value = 0;
int channel;

void loop() {
  int c;

  while (!Serial.available());
  c = Serial.read();
  if ((c >= '0') && (c <= '9')) {
    value = 10 * value + c - '0';
  } else {
    if (c == 'c') channel = value;
    else if (c == 'v') {
      DmxSimple.write(channel, value);
      Serial.print("Ch:");
      Serial.print(channel);
      Serial.print(" Value:");
      Serial.print(value);
      Serial.println();
    }
    value = 0;
  }
}
