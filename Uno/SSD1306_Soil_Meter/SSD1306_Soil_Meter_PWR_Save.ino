/*********************************************************************
  This is an example for reading the analog sensor output of a
  capacitive soil moisture probe and displaying the result on a
  128x64 OLED display over the I2C bus.

  The sensor power is controlled by a P-Channel FET to only power
  it on when it's time to make a measurement, saving battery power.

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

const int wetProbe      = 300;   // wet readings are around 1.5v or analog input value ~300
const int dryProbe      = 620;   // dry readings are around   3v or analog input value ~620


// Analog Sensor Input Pin
#define sensorInput       0     // soil probe is on analog input 0
#define pwrOutputFET      2     // digital pin 2 drives the 5V power rail FET gate
#define pwrOn             LOW   // FET is on when gate is low
#define pwrOff            HIGH  // FET is off when gate is high

// Variables
int validSensorReading  = 0;    // valid sensor analog reading to record
int sensorReading;              // new sensor reading to evaluate and record or discard
int sensorResult;               // scaled sensor data [0..3] = [wet, damp, moist, dry]


void setup()   {

  Serial.begin(9600);                         // debug messages for sensor raw data

  pinMode(pwrOutputFET, OUTPUT);              // FET gate to control sensor power
  digitalWrite(pwrOutputFET, pwrOff);         // power up with sensor power off

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C address of the display
  display.clearDisplay();                     // clear the display buffer
}


void loop() {

  displayTextProbe("Reading...");             // notify that sensor is being powered up and read
  digitalWrite(pwrOutputFET, pwrOn);          // turn sensor power on
  delay(2000);                                // let power rail stabilize
  sensorReading = analogRead(sensorInput);    // take sensor reading
  digitalWrite(pwrOutputFET, pwrOff);         // turn sensor power off

  if (abs(validSensorReading - sensorReading) > 10) {
    validSensorReading = sensorReading;
  }

  Serial.print ("Old Sensor Reading: ");
  Serial.println (validSensorReading);
  Serial.print ("New Sensor Reading: ");
  Serial.println (sensorReading);

  sensorResult = map(validSensorReading, wetProbe, dryProbe, 0, 4);  // scale analog input to a smaller range for wet to dry

  Serial.print ("Scaled Sensor Reading 0-4: ");
  Serial.println (sensorResult);
  Serial.println ();


  // display the correct soil moisture level on the display
  // lower voltages represent more wet levels
  switch (sensorResult) {
    case 0:
      displayTextProbe("Wet");
      break;
    case 1:
      displayTextProbe("Damp");
      break;
    case 2:
      displayTextProbe("Moist");
      break;
    case 3:
      displayTextProbe("Dry");
      break;
    case 4:    // same as case 3, due to how map works.
      displayTextProbe("Dry");
      break;
  }

  delay(3000);                        // display sensor reading for a short duration
  displayTextProbe("Idle...");        // notify that probe is powered off and system is idle

  delay(3000);                        // delay a long time before powering up sensor for another reading
}

void displayTextProbe(const char * sensorDisplay) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.println(sensorDisplay);
  display.display();
  delay(1);
}





