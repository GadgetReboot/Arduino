// YouTube Channel Subscriber Counter with 8 Digit 7-Segment Display
// Target Hardware:  esp8266 WEMOS D1 Mini
//
// Uses the Youtube Data API to retrieve channel statistics
// and display the subscriber count on an 8 digit 7-segment display as well as serial monitor.
//
// Required libraries and board files:
//    esp8266               install from boards manager
//    YoutubeApi            https://github.com/witnessmenow/arduino-youtube-api
//    ArduinoJson           https://github.com/bblanchon/ArduinoJson
//    LedControl            https://github.com/wayoda/LedControl
//
//    LedControl usage:     http://playground.arduino.cc/Main/LedControl
//
// Gadget Reboot

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <YoutubeApi.h>                    // Youtube Data API to retrieve public channel stats
#include <ArduinoJson.h>                   // required by Youtube API
#include <LedControl.h>                    // MAX7219 7-segment display controller

// MAX7219 control pins and number of chips
#define dinPin     D8
#define clkPin     D7
#define csPin      D6
#define numDevices  1                      // one MAX7219 controls 8 digits
#define dispAddr    0                      // MAX7219 devices are numbered starting from 0

// create LED display control interface
LedControl led_disp = LedControl(dinPin, clkPin, csPin, numDevices);

//------- Replace with your details! ------
// char ssid[]      = "myNetworkName";     // your network SSID (name)
// char password[]  = "myNetworkPass";     // your network password
// #define API_KEY    "myAPI_KEY"          // your google apps API key for Youtube Data API access
// #define CHANNEL_ID "myChannelID"        // your Youtube channel ID

unsigned long subCount;                    // stores the subscriber count from the API
byte  digits[] = {0, 0, 0, 0, 0, 0, 0, 0}; // an array of led digit data to be written to the display

// Instantiate the WiFiClientSecure object for use with Youtube API
WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long api_mtbs     = 180000;       // time between api requests, in mS.  60,000 mS = 1 minute intervals
unsigned long api_lasttime = 0;            // last time api request occurred


void setup() {
  Serial.begin(9600);

  // set WiFi to station mode and disconnect from an AP if previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // initialize the MAX7219 device
  led_disp.shutdown(dispAddr, false);         // wake up the MAX7219 from power-saving mode
  led_disp.setIntensity(dispAddr, 8);         // set brightness level (0 - 15)
  led_disp.clearDisplay(dispAddr);            // clear display register

  // Attempt to connect to WiFi network
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    // scan dot right to left
    for (int i = 0; i < 7; i++) {
      led_disp.setChar(dispAddr, i, ' ', true);  // blank digit, dp on
      delay(75);
      led_disp.setChar(dispAddr, i, ' ', false); // blank digit, dp off
    }

    Serial.print(".");

    // scan dot left to right
    for (int i = 7; i > 0; i--) {
      led_disp.setChar(dispAddr, i, ' ', true);  // blank digit, dp on
      delay(75);
      led_disp.setChar(dispAddr, i, ' ', false); // blank digit, dp off
    }
  }  // end while

  // show WiFi status in serial monitor
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // get channel info from Youtube API for initial display
  subCount = 0;        // initial sub count to be updated from api
  pollYoutubeAPI();    // get sub count immediately so we don't wait for poll timer to expire for initial reading
  displayDigits();     // show sub count on 7-segment display

}  // end setup()


void loop() {

  // if it is time to poll the Youtube API, try to retrieve channel data and update display
  if (millis() > api_lasttime + api_mtbs) {
    pollYoutubeAPI();
    displayDigits();   
  }

  // testDisplay();    // uncomment to test the 7-segment digits without api data access
}

/*
 * Other Functions
 */

// show the number on the 7-segment display by separating out the
// individual digits and sending them to the proper LED segment
void displayDigits() {

  // mod math separates digits out of the number
  digits[0] = (subCount % 10);
  digits[1] = (subCount / 10) % 10;
  digits[2] = (subCount / 100) % 10;
  digits[3] = (subCount / 1000) % 10;
  digits[4] = (subCount / 10000) % 10;
  digits[5] = (subCount / 100000) % 10;
  digits[6] = (subCount / 1000000) % 10;
  digits[7] = (subCount / 10000000) % 10;


  // track which digit position in the number contains the left-most (non-zero) digit 
  // eg: number = 474, digits[7..0] = 00000474  left digit non-zero position = digit[2]
  // if there is only one digit in the number, digit position 0 is the left-most automatically
  byte leftDigit = 0;

  if (digits[7] > 0)
    leftDigit = 7;
  else if (digits[6] > 0)
    leftDigit = 6;
  else if (digits[5] > 0)
    leftDigit = 5;
  else if (digits[4] > 0)
    leftDigit = 4;
  else if (digits[3] > 0)
    leftDigit = 3;
  else if (digits[2] > 0)
    leftDigit = 2;
  else if (digits[1] > 0)
    leftDigit = 1;

  // if there are digits to blank out (to the left of the left-most digit in the number)
  // set them as a "space"
  if (leftDigit < 7) {
    for (int i = (leftDigit + 1); i <= 7; i++) {
      digits[i] = ' ';  // blank space character
    }
  }

  // write all digits on 7-segment display
  for (int i = 0; i <= 7; i++)
    led_disp.setChar(dispAddr, i, digits[i], false);

}  // end displayDigits()


void pollYoutubeAPI() {
  if (api.getChannelStatistics(CHANNEL_ID)) {

    Serial.println("---------Stats---------");
    Serial.print("Subscriber Count: ");
    Serial.println(api.channelStats.subscriberCount);
    Serial.print("View Count: ");
    Serial.println(api.channelStats.viewCount);
    Serial.print("Comment Count: ");
    Serial.println(api.channelStats.commentCount);
    Serial.print("Video Count: ");
    Serial.println(api.channelStats.videoCount);
    // Probably not needed :)
    //Serial.print("hiddenSubscriberCount: ");
    //Serial.println(api.channelStats.hiddenSubscriberCount);
    Serial.println("-----------------------");

    subCount = api.channelStats.subscriberCount;
  }

  api_lasttime = millis();   // start a new delay for next API poll interval

}  // end pollYoutubeAPI()


void testDisplay() {

  // display "Arduino"
  led_disp.clearDisplay(dispAddr);
  led_disp.setChar(dispAddr, 7, 'a', false);
  delay(300);
  led_disp.setRow(dispAddr, 6, 0x05);
  delay(300);
  led_disp.setChar(dispAddr, 5, 'd', false);
  delay(300);
  led_disp.setRow(dispAddr, 4, 0x1c);
  delay(300);
  led_disp.setRow(dispAddr, 3, B00010000);
  delay(300);
  led_disp.setRow(dispAddr, 2, 0x15);
  delay(300);
  led_disp.setRow(dispAddr, 1, 0x1D);
  delay(300);
  led_disp.clearDisplay(dispAddr);
  delay(300);

  // test leading zero suppression
  subCount = 1;
  displayDigits(); //Show the data
  delay(300);

  subCount = 22;
  displayDigits(); //Show the data
  delay(300);

  subCount = 333;
  displayDigits(); //Show the data
  delay(300);

  subCount = 4444;
  displayDigits(); //Show the data
  delay(300);

  subCount = 55555;
  displayDigits(); //Show the data
  delay(300);

  subCount = 666666;
  displayDigits(); //Show the data
  delay(300);

  subCount = 7777777;
  displayDigits(); //Show the data
  delay(300);

  subCount = 88888888;
  displayDigits(); //Show the data
  delay(300);
}  // end testDisplay()
