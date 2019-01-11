Using a 480x320 3.5" TFT LCD from MCUFriend with Arduino Uno and a DS18B20 digital thermometer.

Temperature is displayed numerically and graphically on the screen with two touch sensitive 
button areas to switch between deg C and deg F

### General steps to get up and running

1. Install the three required Arduino TFT LCD libraries:
- [MCUFriend_kbv](https://github.com/prenticedavid/MCUFRIEND_kbv)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit TouchScreen](https://github.com/adafruit/Adafruit_TouchScreen)

2. Run the TouchScreen_Calibr_native.ino sketch from the MCUFriend_kbv library example sketches.  This will auto-detect the correct pins for the X and Y plus and minus touch screen pins (YP, YM, XP, XM).  Then use a small tipped stylus to click the screen and enter the next calibration menu.   There will be boxes with cross hairs around the edge of the screen.  Click in the center of the cross hairs to detect the edges and corners of the screen.

3. When the calibration is complete, the serial monitor will display two lines of code that can be directly copied and pasted    into a sketch to configure it for this touch screen's pinouts and screen edge limits.  Copy those lines into the target sketch.  An example may be:
const int XP=7,XM=A1,YP=A2,YM=6; //320x480 ID=0x0099
const int TS_LEFT = 903, TS_RT = 163, TS_TOP = 947, TS_BOT = 165;

4. Some displays may have inverted screen colors, eg. the sketch says to draw a black rectangle but it comes out white.  If this occurs, call the "invertDisplay(true)" routine from somewhere in the setup routine after initializing the screen, for example:
tft.begin(ID);           // initialize the Touch Screen 
tft.invertDisplay(true); // compensate for screens that have inverted colors

This assumes you named your screen object "tft", eg. 
MCUFRIEND_kbv tft;  // create a tft object for the Touch Screen display
