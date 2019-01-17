// YouTube Channel Subscriber Counter
// Target Hardware:  esp8266 NodeMCU
//
// Uses the Youtube Data API to retrieve channel statistics
// and display the subscriber count on an OLED and LCD display as well as serial monitor.
// Can be adapted to display data on other displays.
//
// NodeMCU I2C Pins - defaults used by esp8266 board files in Arduino:
//    SCL:  D1
//    SDA:  D2
//
// Required libraries and board files:
//    esp8266               install from boards manager
//    YoutubeApi            for accessing Youtube via API key
//    ArduinoJson           required by YoutubeAPI
//    Adafruit_SSD1306      for OLED display
//    hd44780               for 16x2 LCD
//
// Gadget Reboot

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>                          // I2C library
#include <YoutubeApi.h>                    // Youtube Data API to retrieve public channel stats
#include <ArduinoJson.h>                   // required by Youtube API
#include <Adafruit_SSD1306.h>              // OLED library
#include <hd44780.h>                       // LCD hd44780 library
#include <hd44780ioClass/hd44780_I2Cexp.h> // I2C expander i/o class 

#define SCREEN_WIDTH 128                   // OLED display width
#define SCREEN_HEIGHT 64                   // OLED display height
#define OLED_RESET    -1                   // Not using this SPI signal so set to -1                        

#define API_KEY    "xxxxxxxxxxxxxxxxxxxx"  // your google apps API key for Youtube Data API access
#define CHANNEL_ID "yyyyyyyyyyyyyyyyyyyy"  // your Youtube channel ID

char ssid[]     = "your network SSID name";  // your network SSID name
char password[] = "your network password";   // your network password

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // declare OLED object

hd44780_I2Cexp lcd; // declare LCD object: auto locate & auto config expander chip

// Instantiate the WiFiClientSecure object for use with Youtube API
WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long api_mtbs     = 60000;        // time between api requests, in mS.  60,000 mS = 1 minute intervals
unsigned long api_lasttime = 0;            // last time api request occurred

void setup() {

  Serial.begin(9600);

  // initialize OLED and print welcome msg
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Joining");
  display.println("WiFi...");
  display.display();

  // initialize 16 char 2 line LCD and print welcome msg
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Joining WiFi...");

  // set WiFi to station mode and disconnect from an AP if previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // show WiFi status in serial monitor
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // update WiFi info on OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi IP: ");
  display.setTextSize(1);
  display.println(ip);
  display.display();

  // update WiFi info on LCD
  lcd.clear();
  lcd.print("WiFi IP:");
  lcd.setCursor(0, 1);        // lower left character position
  lcd.print(ip);

  // show WiFi info on screens for a few seconds before continuing
  delay(3000);

  // get channel info from Youtube API for initial display
  pollYoutubeAPI();

}  // end setup()

void loop() {

  // if it is time to poll the Youtube API, try to retrieve channel data
  if (millis() > api_lasttime + api_mtbs)
    pollYoutubeAPI();

}  // end loop()

//  Other functions/routines

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

    // show subscriber count on OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Gadget");
    display.println("Reboot");
    display.println("Subs: ");
    display.print(api.channelStats.subscriberCount);
    display.display();

    // show subscriber count on LCD
    lcd.clear();
    lcd.print("Gadget Reboot");
    lcd.setCursor(0, 1);     // lower left character position
    lcd.print("Subs:");
    lcd.print(api.channelStats.subscriberCount);
  }

  api_lasttime = millis();   // start a new delay for next API poll interval

}  // end pollYoutubeAPI()


