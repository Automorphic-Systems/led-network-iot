#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <EasyDDNS.h>

#define LED_PIN 7
#define NUM_LEDS 30
#define UCHAR_MAX 255
#define FRAME_INTERVAL 32

/* WiFi Credentials */
const char *ssid = "AutomorphicSystems"; //WIFI ssid
const char *password = "walkingonthemoon"; //WIFI password

CRGB leds[NUM_LEDS];
ESP8266WebServer server(8000);


/* Setup  */
void setup() {
 pinMode(LED_BUILTIN, OUTPUT);
 delay(500);
 Serial.begin(115200);

 setupWifi();

 setupWebHost(); 
 
 setupLEDs();

 /* Do I need these */
 pinMode(D4, OUTPUT);
 digitalWrite(D4, HIGH);
 
 testLEDs();

 Serial.println("Listening for requests!");
}

/* Loop */
void loop() {
  server.handleClient();
} 

/* WiFi Setup */
void setupWifi() {
  Serial.print("Configuring access point...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

/* Web Setup */
void setupWebHost() {
  server.on("/", handleRoot);
  server.on("/led_on", handleLedOn);
  server.on("/led_off", handleLedOff);
  server.on("/set_rgb_color", HTTP_POST, handleSetRGBColor);
  server.on("/set_hsv_color", HTTP_POST, handleSetHSVColor);
  server.on("/set_hsv_color_range", HTTP_POST, handleSetHSVColorRange);
  server.on("/clear", handleClearLeds);
  server.begin();
  Serial.println ( "HTTP server started" );  
}

/* Initialization Test */
void testLEDs() {    
  Serial.println("LED test started");
  unsigned long currTime = 0, prevTime = 0;  
  static uint8_t hue = 0;
  const unsigned long eventInterval = 32;
  
  while(hue < UCHAR_MAX) {         
     currTime = millis();
     if (currTime - prevTime >= eventInterval) {     
        FastLED.showColor(CHSV(hue++, 255, 255)); 
        prevTime = currTime;
        delay(5);
        Serial.println(currTime);
        Serial.println("Color change");
     }
  }
  FastLED.clear();
  FastLED.show();
  Serial.println("LED test completed");
}

/* Setup LEDs */
void setupLEDs() {
   FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
   Serial.println("Activating LEDs"); 
}

/* Web server handlers */
void handleRoot() {
 server.send(200, "text/html", "<h2>Node MCU LED Host</h2><br/>IP Address - " + WiFi.localIP().toString()
                 + "<br/>Number of LEDs: " + NUM_LEDS);
}

void handleSetRGBColor() {
  delay(20); /* debounce */
  Serial.println("handleSetRGBColor");
  if (server.hasArg("pos") && 
      (server.hasArg("rgb") || 
       (server.hasArg("r") && server.hasArg("b") && server.hasArg("g")))) {
    int position = server.arg("pos").toInt();    

    if (!server.hasArg("rgb")) {
       leds[position] = CRGB(server.arg("r").toInt(),server.arg("g").toInt(), server.arg("b").toInt());
       Serial.println("RGB led draw");      
    } else {
       /* need to find an elegant way to convert string to hex RGB code */
       Serial.println("Nothing drawn");
    }
    
    FastLED.show();
    server.send(200, "text/plain","{ result: 1 }");
  } else {
    server.send(400, "text/plain", "Request failed");
  }
}

void handleSetHSVColor() {
   delay(20); /* debounce */
   Serial.println("handleSetHSVColor");
  if (server.hasArg("pos") && 
       server.hasArg("h") && server.hasArg("s") && server.hasArg("v")) {
    int position = server.arg("pos").toInt();   
        
    leds[position] = CHSV(server.arg("h").toInt(),server.arg("s").toInt(), server.arg("v").toInt());     
    Serial.println("HSV led draw");       
    FastLED.show();
    server.send(200, "text/plain","{ result: 1 }");
  } else {
    server.send(400, "text/plain", "Request failed");
  }
}

void handleLedOn() {
    Serial.println("Turning on board LED on..");
    digitalWrite(LED_BUILTIN, 0);
    server.send(200, "text/plain","{ result: 1 }");
}

void handleLedOff() {
    Serial.println("Turning on board LED off..");
    digitalWrite(LED_BUILTIN, 1);
    server.send(200, "text/plain","{ result: 1 }");
}

void handleClearLeds() {
    Serial.println("Resetting all LEDs..");
    FastLED.clear();
    FastLED.show();
    server.send(200, "text/plain","{ result: 1 }");
}

void handleSetHSVColorRange() {
    //delay(50); /* debounce */
    Serial.println("handleSetHSVColorRange");
    if (server.hasArg("pos") && server.hasArg("num") &&
        server.hasArg("h") && server.hasArg("s") && server.hasArg("v")) {
        int pos = server.arg("pos").toInt(); 
        int num = server.arg("num").toInt(); 
        fill_solid( &(leds[pos]), num, CHSV(server.arg("h").toInt(),server.arg("s").toInt(), server.arg("v").toInt()));
        FastLED.show();
        server.send(200, "text/plain","{ result: 1 }");
    } else {
        server.send(400, "text/plain", "Request failed");
    }
}
