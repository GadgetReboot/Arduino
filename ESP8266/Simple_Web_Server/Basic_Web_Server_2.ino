// ESP8266 Web Server Example
// Target Hardware:  esp8266 NodeMCU
//
// Creates a web server over wifi and allows controlling an led and relay 
// while also displaying temperature and humidity data from an SHT-21 sensor.
//
// NodeMCU Pins
//    SCL:    D1    
//    SDA:    D2
//    LED:    D3
//    Relay:  D5
//
// Required libraries and board files:
//    esp8266        install from boards manager
//    SHT21          https://github.com/markbeee/SHT21
//
// Gadget Reboot

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "SHT21.h"

ESP8266WebServer server(80);            // create webserver object to listen for HTTP requests on port 80
SHT21 SHT21;                            // create i2c temperature/humidity sensor object

const char* ssid     = "ssid";         // wifi credentials
const char* password = "password";

#define     ledPin     D3
#define     relayPin   D5

String      htmlPage;                   // webpage text to be sent out by server when main page is accessed

void setup() {
  Serial.begin(9600);

  SHT21.begin();                        // init temp/humidity sensor

  digitalWrite(ledPin, LOW);            // led is off initially
  pinMode(ledPin, OUTPUT);

  digitalWrite(relayPin, HIGH);         // relay is not energized initially (active low)
  pinMode(relayPin, OUTPUT);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();                    // disconnect if previously connected
  delay(100);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // functions to call when client requests are received
  server.on("/", handleRoot);     
  server.on("/LEDTOGGLE", handleLEDToggle);  
  server.on("/LEDON", handleLEDOn);
  server.on("/LEDOFF", handleLEDOff);
  server.on("/RELAYTOGGLE", handleRelayToggle);  
  server.on("/RELAYON", handleRelayOn);
  server.on("/RELAYOFF", handleRelayOff);
  server.onNotFound(handleNotFound);        

  server.begin();                           // start web server
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();                    // listen for HTTP requests from clients
}


// client request handling functions

// send main web page when ip+"/" is accessed
void handleRoot() {
  buildHtmlPage();
  server.send(200, "text/html", htmlPage);
}

// toggle led and redirect to main page
void handleLEDToggle() {                          
  digitalWrite(ledPin, !digitalRead(ledPin));     
  server.sendHeader("Location", "/");       
  server.send(303);                         
}

// turn led on and redirect to main page
void handleLEDOn() {                          
  digitalWrite(ledPin, HIGH);     
  server.sendHeader("Location", "/");       
  server.send(303);                         
}

// turn led off and redirect to main page
void handleLEDOff() {                          
  digitalWrite(ledPin, LOW);     
  server.sendHeader("Location", "/");       
  server.send(303);                         
}

// toggle relay and redirect to main page
void handleRelayToggle() {                         
  digitalWrite(relayPin, !digitalRead(relayPin));     
  server.sendHeader("Location", "/");       
  server.send(303);                         
}

// turn relay on and redirect to main page
void handleRelayOn() {                          
  digitalWrite(relayPin, LOW);     
  server.sendHeader("Location", "/");       
  server.send(303);                         
}

// turn relay off and redirect to main page
void handleRelayOff() {                         
  digitalWrite(relayPin, HIGH);     
  server.sendHeader("Location", "/");       
  server.send(303);                         
}

// send HTTP status 404: Not Found when there's no handler for the client request
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); 
}

// create the html page for the root path of the web server
void buildHtmlPage() {
  htmlPage = "<!DOCTYPE html>";
  htmlPage += "<html>";
  htmlPage += "<head>";                                        // header section
  htmlPage += "<title>ESP8266 Web Server</title>";             // title for browser window
  htmlPage += "<meta http-equiv=\"refresh\" content=\"10\">";  // auto-refresh the page every 10 seconds to show new data
  htmlPage += "<meta charset=\"UTF-8\">";                      // UTF-8 allows the degree symbol to display correctly
  htmlPage += "</head>";
  
  htmlPage += "<BODY bgcolor='#E0E0D0'>";                      // body section, set background color

  // retrieve temperature/humidity from sensor to display on main page
  htmlPage += "<br>Temperature °C: " + String(SHT21.getTemperature());
  htmlPage += "<br>Humidity   %RH: " + String(SHT21.getHumidity());
  htmlPage += "<br>";

  // show led status and action buttons
  String ledState = ((digitalRead(ledPin)) ? "on" : "off");
  htmlPage += "<br>LED: " + ledState;
  htmlPage += "<form action=\"/LEDON\" method=\"POST\"><input type=\"submit\" value=\"LED On\"></form>";
  htmlPage += "<form action=\"/LEDOFF\" method=\"POST\"><input type=\"submit\" value=\"LED Off\"></form>";
  htmlPage += "<form action=\"/LEDTOGGLE\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>";

  // show relay status and action buttons
  String relayState = ((!digitalRead(relayPin)) ? "on" : "off");
  htmlPage += "<br>Relay: " + relayState;
  htmlPage += "<form action=\"/RELAYON\" method=\"POST\"><input type=\"submit\" value=\"Relay On\"></form>";
  htmlPage += "<form action=\"/RELAYOFF\" method=\"POST\"><input type=\"submit\" value=\"Relay Off\"></form>";
  htmlPage += "<form action=\"/RELAYTOGGLE\" method=\"POST\"><input type=\"submit\" value=\"Toggle Relay\"></form>";

  htmlPage += "</body>";
  htmlPage += "</html>";
}
