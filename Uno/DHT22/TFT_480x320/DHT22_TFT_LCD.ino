// Work in progress sketch, needs to be cleaned up and possibly tweaked,
// check back for updates...

// Using a DHT22 Temp/Humidity sensor with an MCUFriend 3.5" 480x320 TFT LCD
//
// Graphical display inspired by the sketch/project:
//     Bar Graph Digital Thermometer Using TFT LCD Touch Screen (Arduino Based)
//     http://basicarduinotutorial.blogspot.com/2017/08/project-ii-16-bar-graph-digital.html
//
// On-screen button interface is based on the button_simple.ino demo sketch included with
// the MCUFRIEND_kbv library
//
// For more details and a schematic, refer to
// https://github.com/GadgetReboot/Arduino/tree/master/Uno/DS18B20_TFT_LCD
//
// This code is in the public domain
//
// Gadget Reboot

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"

#define DHTPIN 19     // Digital pin connected to the DHT sensor

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

// run TouchScreen_Calibr_native.ino in MCUFRIEND_kbv examples to calibrate screen
// copy calibration data from serial monitor for XP, XM, YP, YM, TS_LEFT, TS_RT, TS_TOP, TS_BOT
const int XP = 7, XM = A1, YP = A2, YM = 6; //320x480 ID=0x0099
const int TS_LEFT = 903, TS_RT = 163, TS_TOP = 947, TS_BOT = 165;

MCUFRIEND_kbv tft;                                 // create display object
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // create touch screen object

// assign human-readable names to some common 16-bit color values:
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF
#define GRAY     0xC618
#define DARKGRAY 0x6B0C

// thermometer graphic coordinates and size
#define thermXTop      155  // thermometer tube top center x coordinate
#define thermYTop      130  // thermometer tube top center y coordinate
#define thermHeight    200  // thermometer tube height
#define thermTubeWidth  30  // thermometer tube width
#define thermBulbRadius 30  // thermometer bulb radius

// touch pressure sensitivity thresholds to detect screen presses
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// used to control/check if units should be displayed in C or F (makes code more readable)
#define  degC false
#define  degF true

uint16_t tsID;                                          // touch screen driver type ID
int           pixel_x;                                  // touch screen pressed location
int           pixel_y;                                  // touch screen pressed location
uint8_t       Orientation      = 0;                     // portrait screen orientation
int           prevTempLevel    = 0;                     // store last thermometer reading level
bool          displayTempUnits = degC;                  // default to displaying in degrees C
unsigned long timeInterval     = 2000;                  // time interval between DHT readings in mS
unsigned long timeLapsed;                               // time lapsed since last timer expiry

// create two touch screen buttons
Adafruit_GFX_Button degC_btn;
Adafruit_GFX_Button degF_btn;

void setup(void)
{
  Serial.begin(9600);

  // initialize the DHT sensor
  dht.begin();

  // initialize Touch Screen
  tft.reset();
  tsID = tft.readID();                                  // find what driver is on the TFT LCD
  tft.begin(tsID);
  tft.invertDisplay(true);                              // invert screen colors if this driver requires it
  tft.setRotation(Orientation);                         // set portrait or landscape mode
  tft.fillScreen(BLACK);                                // clear the screen

  // set up two on-screen buttons to switch between deg C and F
  degC_btn.initButton(&tft,  80, 430, 100, 40, WHITE, CYAN, BLACK, "deg C", 2);
  degF_btn.initButton(&tft, 240, 430, 100, 40, WHITE, CYAN, BLACK, "deg F", 2);
  degC_btn.drawButton(false);
  degF_btn.drawButton(false);

  // draw display boxes on screen
  tft.drawRect(10, 10, 300, 75, WHITE);     // header and digital temp readout box
  tft.fillRect(15, 15, 290, 65, DARKGRAY);

  tft.setTextColor(WHITE, DARKGRAY);        // printer header text
  tft.setTextSize(2);
  tft.setCursor(35, 25);
  tft.print("Temperature  Humidity");

  tft.drawRect(10, 90, 300, 300, WHITE);    // main thermometer graphic display box
  tft.fillRect(15, 95, 290, 290, GRAY);
  drawThermometer();                        // draw static thermometer background graphics
}

void loop()
{
  // if it is time to read from the DHT sensor, take a temp/humidity reading
  if (millis() - timeLapsed > timeInterval)  {
    // Reading temperature or humidity takes about 250 milliseconds!
    float humidity = dht.readHumidity();
    // Read temperature: displayTempUnits true=degF false=degC
    float displayTemp = dht.readTemperature(displayTempUnits);

    // Check if any reads failed
    if (isnan(humidity) || isnan(displayTemp)) {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
    else {
      Serial.print(F("Humidity: "));
      Serial.print(humidity);
      Serial.print(F("%  Temperature: "));
      Serial.print(displayTemp);
      if (displayTempUnits == degC) {
        Serial.print(F("°C "));
      }
      else {
        Serial.print(F("°F "));
      }
      Serial.println();
    }

    // display temperature numerically in header box
    tft.setTextColor(WHITE, DARKGRAY);
    tft.setTextSize(3);
    tft.setCursor(50, 50);
    tft.print(displayTemp, 1);                // show temperature with 1 decimal place
    tft.print((char)247);                     // degree symbol
    if (displayTempUnits == degC)
      tft.print("C  ");
    else
      tft.print("F  ");

    tft.setCursor(195, 50);
    tft.print(humidity, 1);                   // show humidity with 1 decimal place
    tft.print("%");

    // update thermometer level graphic if temperature is within graphical range
    if ((displayTemp <= 100 ) && (displayTemp >= 0 )) {
      int curTempLevel = map(displayTemp, 0, 100, 0, (thermHeight - (thermBulbRadius)));

      // update thermometer level if height has changed
      if ((prevTempLevel - curTempLevel) != 0) {
        // draw new red level
        tft.fillRect((thermXTop - (thermTubeWidth / 2) + 6),
                     thermYTop + ((thermHeight - (thermBulbRadius) - curTempLevel)),
                     (thermTubeWidth - 12),
                     curTempLevel,
                     RED);

        // draw new white background above red level height in tube
        tft.fillRect((thermXTop - (thermTubeWidth / 2) + 3),
                     thermYTop,
                     (thermTubeWidth - 6), (thermHeight - (thermBulbRadius) - curTempLevel),
                     WHITE);
      }
      prevTempLevel = curTempLevel; // store bar height for next iteration
    }
    timeLapsed = millis();
  }

  // check if either of the on screen buttons are pressed and
  // change between deg C and F display as required
  bool buttonPressed = Touch_getXY();
  degC_btn.press(buttonPressed && degC_btn.contains(pixel_x, pixel_y));
  degF_btn.press(buttonPressed && degF_btn.contains(pixel_x, pixel_y));

  if (degC_btn.justReleased())    // switch to showing degrees C
    degC_btn.drawButton();
  if (degC_btn.justPressed()) {
    degC_btn.drawButton(true);
    displayTempUnits = degC;
  }

  if (degF_btn.justReleased())    // switch to showing degrees F
    degF_btn.drawButton();
  if (degF_btn.justPressed()) {
    degF_btn.drawButton(true);
    displayTempUnits = degF;
  }
}

// get touch screen pixel location if screen is touched
bool Touch_getXY(void)
{
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);      // restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);   // because TFT control pins
  digitalWrite(XM, HIGH);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());  // map x location from analog pos. reading to screen width
    pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height()); // map y location from analog pos. reading to screen height
  }
  return pressed;
}

// draw the background graphics for the thermometer and markings
// this is only drawn once, then the temperature bar is adjusted as needed
void drawThermometer(void)
{
  // draw thermometer tube outline
  tft.fillRoundRect((thermXTop - (thermTubeWidth / 2)), thermYTop - (thermTubeWidth / 2) + 1,
                    thermTubeWidth, thermHeight, thermTubeWidth / 2, BLACK);                             // tube
  tft.fillCircle(thermXTop, (thermYTop + thermHeight), (thermBulbRadius), BLACK);                        // bottom bulb

  // fill white thermometer tube inside
  tft.fillRoundRect((thermXTop - (thermTubeWidth / 2) + 3), thermYTop - (thermTubeWidth / 2) + 5,
                    (thermTubeWidth - 6), thermHeight, (thermTubeWidth / 2) - 0.5, WHITE);               // tube
  tft.fillCircle(thermXTop, (thermYTop + thermHeight), (thermBulbRadius - 3), WHITE);                    // bottom bulb

  // fill red thermometer bulb
  tft.fillCircle(thermXTop, (thermYTop + thermHeight), (thermBulbRadius - 6), RED);

  // fill red tube to connect between lowest reading level and the bulb reservoir
  tft.fillRect((thermXTop - (thermTubeWidth / 2) + 6), thermYTop + thermHeight - thermBulbRadius, (thermTubeWidth - 12), 7, RED);

  // draw line markings beside thermometer for visual reference levels
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, thermYTop, 8, 2, BLACK);                                             // top level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.75), 5, 2, BLACK);  // 1/4 from top
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.50), 8, 2, BLACK);  // center
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.25), 5, 2, BLACK);  // 1/4 from bot
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, ((thermYTop + thermHeight) - thermBulbRadius), 8, 2, BLACK);         // bottom level

  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.95), 3, 2, BLACK);  //  "5" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.90), 3, 2, BLACK);  // "10" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.85), 3, 2, BLACK);  // "15" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.80), 3, 2, BLACK);  // "20" level

  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.70), 3, 2, BLACK);  // "30" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.65), 3, 2, BLACK);  // "35" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.60), 3, 2, BLACK);  // "40" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.55), 3, 2, BLACK);  // "45" level

  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.45), 3, 2, BLACK);  // "55" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.40), 3, 2, BLACK);  // "60" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.35), 3, 2, BLACK);  // "65" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.30), 3, 2, BLACK);  // "70" level

  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.20), 3, 2, BLACK);  // "80" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.15), 3, 2, BLACK);  // "85" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.10), 3, 2, BLACK);  // "90" level
  tft.fillRect(thermXTop + (thermTubeWidth / 2) + 1, (thermYTop + (thermHeight - thermBulbRadius) * 0.05), 3, 2, BLACK);  // "95" level

  // draw thermometer level indicator numbers
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(((thermXTop + (thermTubeWidth / 2)) + 10), (thermYTop - 5));
  tft.print("100");
  tft.setTextSize(2);
  tft.print((char)247);

  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(((thermXTop + (thermTubeWidth / 2)) + 10), (thermYTop + (thermHeight - thermBulbRadius) * 0.25) - 5);
  tft.print("75");
  tft.setTextSize(2);
  tft.print((char)247);

  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(((thermXTop + (thermTubeWidth / 2)) + 10), (thermYTop + (thermHeight - thermBulbRadius) * 0.5) - 5);
  tft.print("50");
  tft.setTextSize(2);
  tft.print((char)247);

  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(((thermXTop + (thermTubeWidth / 2)) + 10), (thermYTop + (thermHeight - thermBulbRadius) * 0.75) - 5);
  tft.print("25");
  tft.setTextSize(2);
  tft.print((char)247);

  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(((thermXTop + (thermTubeWidth / 2)) + 10), (thermYTop + thermHeight - thermBulbRadius - 5));
  tft.print("0");
  tft.setTextSize(2);
  tft.print((char)247);
}
