// MAX7219 8x8 LED Scrolling Sign
// with Youtube subscriber counter and optional user defined text
// entered via a web page hosted at the ESP8266 web server ip
// ip address for the ESP8266 is shown in the serial monitor
// after joining wifi network
//
// Based on original project from https://www.etechpath.com/how-to-control-max7219-led-matrix-with-esp8266-wifi-module/
// Various tweaks including Youtube API integration were added.
//
// Target Hardware:  esp8266 NodeMCU
//                   MAX7219 based 8x8 LED dot matrix displays (4 used in demo)
//
// Uses the Youtube Data API to retrieve channel statistics
//
// Connections between MAX7219 and NodeMCU:
// Vcc        5V     LED matrix is designed for 5V, may run at 3.3v but may be unreliable
// GND       GND     GND
// DIN        D7     MOSI
// CS         D8     CS
// CLK        D5     CLK
//
// Required libraries and board files:
//    esp8266               install from boards manager
//    YoutubeApi            access Youtube via API key       https://github.com/witnessmenow/arduino-youtube-api
//    ArduinoJson           required by YoutubeAPI           https://github.com/bblanchon/ArduinoJson
//    MD_MAX72xx.h          MAX7219 scrolling matrix library https://github.com/MajicDesigns/MD_MAX72XX
//
// Gadget Reboot

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MD_MAX72xx.h>         // MAX7219 scrolling matrix library
#include <YoutubeApi.h>         // youtube data api to retrieve public channel stats
#include <ArduinoJson.h>        // required by Youtube API

//wifi login, youtube api key, and youtube channel id
const char* ssid     = "yournetworkname";                       // your wifi network name
const char* password = "yourpassword";                          // your wifi password
#define API_KEY        "yourapikey"                             // your google apps API key for youtube data api
#define CHANNEL_ID     "yourchannelid"                          // your youtube channel id

// define the number of 8x8 matrix displays in the chain
#define	MAX_DEVICES	4

// SPI pins
#define	CLK_PIN		  D5
#define	DATA_PIN	  D7
#define	CS_PIN		  D8

// configure SPI hardware interface based on matrix style
// from MD_MAX72xx.h the options are:
//       PAROLA_HW   Use the Parola style hardware module
//      GENERIC_HW   Use 'generic' style hardware modules commonly available
//    ICSTATION_HW   Use ICStation style hardware module
//         FC16_HW   Use FC-16 style hardware module
#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// create wifi server object for web message text entry
// and a secure client for the youtube api
WiFiServer server(80);
WiFiClientSecure YTclient;
YoutubeApi api(API_KEY, YTclient);

unsigned long api_mtbs     = 60000;        // time between api requests, in mS.  60,000 mS = 1 minute intervals
unsigned long api_lasttime = 0;            // last time api request occurred
int           lastSubCount = 0;            // if subscriber count has changed, time to update message text

// Global message buffers shared by wifi and message scrolling functions
const uint8_t msgLength     = 255;  // total length of scrolling message ***webMsg+statMsg length must be <= msgLength***
const uint8_t webMsgLength  = 201;  // length of youtube stats text incl. null terminator
const uint8_t statMsgLength = 54;   // length of youtube stats text incl. null terminator

char curMessage[msgLength];         // combined message to scroll
char webMessage[webMsgLength];      // partial message - variable text from web entry
char statsMessage[statMsgLength];   // partial message - youTube stats

const uint8_t charSpacing   = 1;
const uint8_t scrollDelay   = 120;  // message scroll speed
bool webMessageAvailable = false;

// build web page for message entry form
// the number for "maxlength" for the "label" in the webpage below
// must be no larger than webMsgLength
char webResponse[] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
char webPage[] =
  "<!DOCTYPE html>" \
  "<html>" \
  "<head>" \
  "<title>ESP8266 Scrolling Text</title>" \
  "<style>" \
  "html, body" \
  "{" \
  "width: 600px;" \
  "height: 200px;" \
  "margin: 0px;" \
  "border: 0px;" \
  "padding: 10px;" \
  "background-color: white;" \
  "}" \
  "#container " \
  "{" \
  "width: 100%;" \
  "height: 100%;" \
  "margin-left: 50px;" \
  "border: solid 2px;" \
  "padding: 10px;" \
  "background-color: #d1d1f1;" \
  "}" \
  "</style>"\
  "<script>" \
  "strLine = \"\";" \
  "function SendText()" \
  "{" \
  "  nocache = \"/&nocache=\" + Math.random() * 1000000;" \
  "  var request = new XMLHttpRequest();" \
  "  strLine = \"&MSG=\" + document.getElementById(\"txt_form\").Message.value;" \
  "  request.open(\"GET\", strLine + nocache, false);" \
  "  request.send(null);" \
  "}" \
  "</script>" \
  "</head>" \
  "<body>" \
  "<div id=\"container\">"\
  "<H1><b>MAX7219 LED Matrix Custom Text</b></H1>" \
  "<form id=\"txt_form\" name=\"frmText\">" \
  "<label>Enter Custom Message: <input type=\"text\" name=\"Message\" maxlength=\"200\"></label><br>" \
  "<H5>MAX 200 characters</H5>" \
  "</form>" \
  "<input type=\"submit\" value=\"Submit Text\" onclick=\"SendText()\">" \
  "</div>" \
  "</body>" \
  "</html>";

void setup() {
  Serial.begin(9600);
  delay(200);          // give time for serial to connect or some initial debug characters may be missed

  Serial.print("\n\nMAX7219 scrolling matrix sign with YouTube sub counter and optional wifi message.");
  Serial.print("\nEnter wifi ip in a browser to add custom message.");

  // init scroll messages as null strings
  curMessage[0] = webMessage[0] = statsMessage[0] = '\0';

  // init display matrix
  mx.begin();
  mx.setShiftDataInCallback(scrollDataSource);

  // set wifi to station mode and disconnect from an AP if previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // join wifi network
  Serial.print("\n\nConnecting to wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nwifi connected.");
  Serial.print("ip address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // start web server
  server.begin();
  Serial.print("\nServer started.\n\n");

  pollYoutubeAPI();  // get channel info from youtube api for immediate use (main loop imposes wait period)
  updateMessage();   // update scrolling message text with latest api stats/web messages
}  // end setup

void loop() {
  // if it is time to poll the Youtube API, try to retrieve channel data
  if (millis() > api_lasttime + api_mtbs)
    pollYoutubeAPI();

  handleWiFi();  // deal with web server requests
  scrollText();  // deal with scrolling sign tasks
}  // end loop


// join all message text strings into the current message to scroll
void updateMessage() {
  // build youtube stats message string
  String s = "YouTube Subs: " + String(lastSubCount);
  s.toCharArray(statsMessage, statMsgLength);

  // copy custom web text into beginning of current message string
  // then concatenate youtube stats message at the end
  // ie. curMessage = webMessage + statsMessage
  strcpy(curMessage, webMessage);
  strcat(curMessage, statsMessage);
}  // end updateMessage


// retrieve youtube channel stats over wifi through the api
// and update the sub count in the scrolling message text
void pollYoutubeAPI() {
  if (api.getChannelStatistics(CHANNEL_ID)) {

    Serial.println("-----YouTube Stats-----");
    Serial.print("Subscriber Count: ");
    Serial.println(api.channelStats.subscriberCount);
    Serial.print("Channel Views: ");
    Serial.println(api.channelStats.viewCount);
    Serial.print("Video Count: ");
    Serial.println(api.channelStats.videoCount);
    Serial.println("-----------------------");

    if (lastSubCount != api.channelStats.subscriberCount) {
      lastSubCount = api.channelStats.subscriberCount;
      updateMessage();
    }
  }
  api_lasttime = millis();   // start a new delay for next API poll interval
}  // end pollYoutubeAPI


// convert hex character received as ASCII characters into integers
// eg ASCII symbol "0" is ASCII table position 48
//    ASCII symbol "2" is ASCII table position 50
//    ASCII symbol "A" is ASCII table position 65
//    ASCII symbol "B" is ASCII table position 66
//    "0" to integer is symbol "0" (ascii 48) minus symbol "0" (ascii 48) = (48 - 48) = integer 0
//    "2" to integer is symbol "2" (ascii 50) minus symbol "0" (ascii 48) = (50 - 48) = integer 2
//    "B" to integer is symbol "B" (ascii 66) minus symbol "A" (ascii 65) + integer 10
//                   = (66 - 65 + 10) = integer 11 = 0x0b
uint8_t htoi(char c) {
  c = toupper(c);
  if ((c >= '0') && (c <= '9')) return (c - '0');
  if ((c >= 'A') && (c <= 'F')) return (c - 'A' + 10);
  return (0);
} // end htoi


// get message string from web text entry box to use in scrolling message
boolean getText(char *szMesg, char *psz, uint8_t len) {
  boolean isValid = false;  // text received flag
  char *pStart, *pEnd;      // pointer to start and end of text

  // get pointer to the beginning of the text
  pStart = strstr(szMesg, "/&MSG=");

  if (pStart != NULL)
  {
    pStart += 6;  // skip to start of data
    pEnd = strstr(pStart, "/&");

    if (pEnd != NULL)
    {
      while (pStart != pEnd)
      {
        if ((*pStart == '%') && isdigit(*(pStart + 1)))
        {
          // replace %xx hex code with the ASCII character
          char c = 0;
          pStart++;
          c += (htoi(*pStart++) << 4);
          c += htoi(*pStart++);
          *psz++ = c;
        }
        else
          *psz++ = *pStart++;
      }

      *psz = '\0'; // terminate the string
      isValid = true;
    }
  }
  return (isValid);
}  // end getText

// show web page to clients and process text form input
void handleWiFi(void) {
  static enum { S_IDLE, S_WAIT_CONN, S_READ, S_EXTRACT, S_RESPONSE, S_DISCONN } state = S_IDLE;
  static char szBuf[1024];
  static uint16_t idxBuf = 0;
  static WiFiClient webclient;
  static uint32_t timeStart;

  switch (state)
  {
    case S_IDLE:   // initialise
      idxBuf = 0;
      state = S_WAIT_CONN;
      break;

    case S_WAIT_CONN:   // waiting for connection
      {
        webclient = server.available();
        if (!webclient) break;
        if (!webclient.connected()) break;

        timeStart = millis();
        state = S_READ;
      }
      break;

    case S_READ: // get the first line of data
      while (webclient.available())
      {
        char c = webclient.read();
        if ((c == '\r') || (c == '\n'))
        {
          szBuf[idxBuf] = '\0';
          webclient.flush();
          state = S_EXTRACT;
        }
        else
          szBuf[idxBuf++] = (char)c;
      }
      if (millis() - timeStart > 1000)
      {
        state = S_DISCONN;
      }
      break;

    case S_EXTRACT: // extract data
      // Extract the string from the message if there is one
      webMessageAvailable = getText(szBuf, webMessage, msgLength);
      state = S_RESPONSE;
      break;

    case S_RESPONSE: // send the response to the client
      // Return the response to the client (web page)
      webclient.print(webResponse);
      webclient.print(webPage);
      state = S_DISCONN;
      break;

    case S_DISCONN: // disconnect client
      webclient.flush();
      webclient.stop();
      state = S_IDLE;
      break;

    default:  state = S_IDLE;
  }
}  // end handleWiFi


uint8_t scrollDataSource(uint8_t dev, MD_MAX72XX::transformType_t t)
// Callback function for data that is required for scrolling into the display
{
  static enum { S_IDLE, S_NEXT_CHAR, S_SHOW_CHAR, S_SHOW_SPACE } state = S_IDLE;
  static char    *p;
  static uint16_t curLen, showLen;
  static uint8_t  cBuf[8];
  uint8_t colData = 0;

  // finite state machine to control what we do on the callback
  switch (state)
  {
    case S_IDLE: // reset the message pointer and check for new message to load
      p = curMessage;      // reset the pointer to start of message
      if (webMessageAvailable)  // there is a new message waiting
      {
        updateMessage();
        webMessageAvailable = false;
      }
      state = S_NEXT_CHAR;
      break;

    case S_NEXT_CHAR: // Load the next character from the font table
      if (*p == '\0')
        state = S_IDLE;
      else
      {
        showLen = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state = S_SHOW_CHAR;
      }
      break;

    case S_SHOW_CHAR: // display the next part of the character
      colData = cBuf[curLen++];
      if (curLen < showLen)
        break;

      // set up the inter character spacing
      showLen = (*p != '\0' ? charSpacing : (MAX_DEVICES * COL_SIZE) / 2);
      curLen = 0;
      state = S_SHOW_SPACE;
    // fall through

    case S_SHOW_SPACE:  // display inter-character spacing (blank column)
      curLen++;
      if (curLen == showLen)
        state = S_NEXT_CHAR;
      break;

    default:
      state = S_IDLE;
  }
  return (colData);
}  // end scrollDataSource


void scrollText(void) {
  static uint32_t prevTime = 0;

  // Is it time to scroll the text?
  if (millis() - prevTime >= scrollDelay)
  {
    mx.transform(MD_MAX72XX::TSL);  // scroll along - the callback will load all the data
    prevTime = millis();      // starting point for next time
  }
}  // end scrollText
