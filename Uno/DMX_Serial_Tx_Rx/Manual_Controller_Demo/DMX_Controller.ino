// DMX Controller Example
// Target Hardware:  Arduino Uno
//
// Reads in two analog slider potentiometers and sends a value from 1 to 255
// for each slider out to a DMX channel, selected by four push buttons.
//
// One push button toggles the target DMX main channel between two light fixtures
// The first slider pot controls the master brightness on the selected light fixture
//
// Three push buttons select between red, green, and blue for slider pot #2
// which will control the chosen color level for the current light fixture
//
// Required library:
//    DMXSerial        install from library manager or https://github.com/mathertel/DMXSerial
//    Bounce2          install from library manager
//
// Gadget Reboot
// https://www.youtube.com/gadgetreboot

#include <DMXSerial.h>
#include <Bounce2.h>              // button debounce library

#define dmxModePin           2    // RS485 data direction control
#define masterBrightnessPin A0    // slider pot #1 for current fixture master brightness
#define channelLevelPin     A1    // slider pot #2 for current fixture chosen color level
#define redCtrlBtn           6    // button: slider pot #2 controls red
#define grnCtrlBtn           5    // button: slider pot #2 controls green
#define bluCtrlBtn           4    // button: slider pot #2 controls blue
#define fixtureToggleBtn     3    // button: toggle current DMX fixture to control

byte fixtureOneCh        =   5;   // first  light fixture base channel
byte fixtureTwoCh        =  10;   // second light fixture base channel
byte masterBrightness;            // master brightness level
byte channelLevel;                // channel level data
byte targetFixture;               // current fixture being controlled
byte targetRGBChannel;            // current RGB channel offset (from base ch) red=0 grn=1 blu=2
byte brightnessChannel   =   4;   // brightness is channel 4 offset from the fixture base channel

const int debounceInterval = 10;  // debounce time (ms) for button readings
Bounce buttonRedDB  = Bounce();   // instantiate a bounce object for each button
Bounce buttonGrnDB  = Bounce();
Bounce buttonBluDB  = Bounce();
Bounce buttonFixtureDB  = Bounce();

void setup() {
  DMXSerial.init(DMXController, dmxModePin);  // start the DMX master on the UART
  targetFixture = fixtureOneCh;               // start out controlling the first light fixture
  targetRGBChannel = 0;                       // start out controlling the red level with slider pot #2

  pinMode(redCtrlBtn, INPUT_PULLUP);          // configure push buttons
  pinMode(grnCtrlBtn, INPUT_PULLUP);
  pinMode(bluCtrlBtn, INPUT_PULLUP);
  pinMode(fixtureToggleBtn, INPUT_PULLUP);

  // attach buttons to debouncers
  buttonRedDB.attach(redCtrlBtn);
  buttonRedDB.interval(debounceInterval);

  buttonGrnDB.attach(grnCtrlBtn);
  buttonGrnDB.interval(debounceInterval);

  buttonBluDB.attach(bluCtrlBtn);
  buttonBluDB.interval(debounceInterval);

  buttonFixtureDB.attach(fixtureToggleBtn);
  buttonFixtureDB.interval(debounceInterval);

  // set default master brightness levels on both fixtures to 100 out of 255
  // the brightness channel is 4 channels above the base channel of each fixture
  // eg Fixture 1 is at base channel 5 so brightness is controlled by channel 9
  DMXSerial.write(fixtureOneCh + brightnessChannel, 100);
  DMXSerial.write(fixtureTwoCh + brightnessChannel, 100);
}

void loop() {

  // read slider pots and map a value from 1 to 255 to control
  // master brightness and color level, to be sent to the current
  // light fixture
  masterBrightness = map(analogRead(masterBrightnessPin), 0, 1023, 1, 255);
  channelLevel = map(analogRead(channelLevelPin), 0, 1023, 1, 255);

  // read the debounced push buttons and if one is pressed, assign
  // the current RGB color to control with slider pot #2, or assign
  // the current light fixture to control
  readButtons();

  // send the DMX control data to the current light fixture to set the
  // individual RGB color levels and the master brightness
  DMXSerial.write(targetFixture + targetRGBChannel, channelLevel);
  DMXSerial.write(targetFixture + brightnessChannel, masterBrightness);
}

void readButtons() {

  // update the Bounce instances
  buttonRedDB.update();
  buttonGrnDB.update();
  buttonBluDB.update();
  buttonFixtureDB.update();

  if ( buttonRedDB.fell()) {
    targetRGBChannel = 0;
  }

  if ( buttonGrnDB.fell()) {
    targetRGBChannel = 1;
  }

  if ( buttonBluDB.fell()) {
    targetRGBChannel = 2;
  }

  if ( buttonFixtureDB.fell()) {
    if (targetFixture == fixtureOneCh)
      targetFixture = fixtureTwoCh;
    else if (targetFixture == fixtureTwoCh)
      targetFixture = fixtureOneCh;
  }
}
