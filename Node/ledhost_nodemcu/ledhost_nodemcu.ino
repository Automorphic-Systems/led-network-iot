#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>

#define LED_PIN 7
#define NUM_LEDS 288
#define UCHAR_MAX 255
#define FRAME_INTERVAL 32
#define HSV_DEFAULT_VALUE 160

/* WiFi Credentials */
const char *ssid = "AutomorphicSystems"; //WIFI ssid
const char *password = "walkingonthemoon"; //WIFI password

/* HTTP Setup */
ESP8266WebServer server(8000);

/* LED and heatmap values*/
CRGB leds[NUM_LEDS];
uint8_t leds_h[NUM_LEDS];
CRGB seedLeds[NUM_LEDS];
CRGBPalette16 currentPalette;
static int heatmap[NUM_LEDS];

/* Logging */
char logbuffer [80];
int logsize;
bool cycle;
char *modes[4] = {"rainbow", "streak", "chase", "flicker"};
char *currentMode;
char *displayType[2] = { "frame", "mode" };
char *currentDisplay;

/* Timing */
unsigned long currTime, prevTime, startTime, cycleTime;

/* Mode Parameters */
uint8_t hue;
uint16_t pos;
int16_t offset;
uint8_t seeds, pal_cols;
int8_t chase_rate;
int16_t iter, maxiter;
uint8_t fade_rate, boost_rate, boost_thr, twink_thr, twink_min, twink_max, seed_amt;

/* Setup  */
void setup() {

    /* Set pins, clear UART */  
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(D4, OUTPUT);
    delay(2000);
    Serial.begin(115200);
    delay(500);
    Serial.flush();
    while ( Serial.available() ) Serial.read(); 

    
    Serial.println("Initializing parameters, activating LEDs");

    /* General */
    pos, hue, currTime, prevTime, startTime, offset, iter = 0;
    pal_cols = 2;
    maxiter = NUM_LEDS;

    /* Chase */
    chase_rate = 2;

    /* Flicker */
    fade_rate = 6;
    boost_rate = 5;
    seed_amt = 20;
    boost_thr = 100;
    twink_thr = 32;
    twink_min = 48;
    twink_max = 160;

    /* Connect to Wi-fi */
    setupWifi();

    /* Set up web server */
    setupWebHost(); 
 
    /* Set LED strip */
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

    currentDisplay = displayType[1];
    currentMode = modes[0];

}

/* Loop */
void loop() {
    currTime = millis();
    
    /* Process requests to change parameters */
    server.handleClient();

    /* Render the display */
    if (currTime - prevTime > FRAME_INTERVAL) {
      
        FastLED.show();       
        prevTime = currTime;      
    }

    /* Log after each complete cycle of frames */
    if (cycle) {    
        logsize = sprintf(logbuffer, "display: %s, mode: %s, rate: %i, leds: %i, time: %i ms", currentDisplay, currentMode, FRAME_INTERVAL, NUM_LEDS, millis() - cycleTime);
        Serial.println(logbuffer);
        cycleTime = millis();
        cycle = 0;
    }  
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
    Serial.println("WiFi connected \n\r\ IP address: ");
    Serial.println(WiFi.localIP());  
}

/* Web Setup */
void setupWebHost() {
  server.on("/", handleRoot);
  server.on("/clear", handleClearLeds);
  server.on("/set_config", HTTP_POST, handleConfig);
  server.on("/set_frame", HTTP_POST, handleFrame);
  server.on("/set_mode", HTTP_POST, handleMode);
  server.begin();
  Serial.println ( "HTTP server started" );  
}


/* Web server handlers */
void handleRoot() {
    server.send(200, "text/html", "<h2>Node MCU LED Host</h2><br/>IP Address - " + WiFi.localIP().toString()
                 + "<br/>Number of LEDs: " + NUM_LEDS
                 + "<br/>Frame internval: " + FRAME_INTERVAL + " ms");
}

void handleClearLeds() {
    Serial.println("Resetting all LEDs..");
    FastLED.clear();
    server.send(200, "text/plain","{ result: 1 }");
}


void handleMode() {
  
    server.send(200, "text/plain","{ result: 1 }");
}

void handleFrame() {
  
    server.send(200, "text/plain","{ result: 1 }");
}

void handleConfig() {
  
    server.send(200, "text/plain","{ result: 1 }");
}


/* Helper Methods */
void generateRandomFrame(CRGB frame[]) {    
  for (int i = 0; i < NUM_LEDS; i++) {
    random16_add_entropy( random(UCHAR_MAX));
    frame[i].setHSV(random(UCHAR_MAX), random(UCHAR_MAX), random(HSV_DEFAULT_VALUE));
  }
}

void generateRandomFrameFromPalette(CRGB frame[]) {  
  currentPalette = generateRandomPalette(pal_cols);
  
  uint8_t colIdx = 1;
  for (int i = 0; i < NUM_LEDS; i++) {
    frame[i] = ColorFromPalette( currentPalette, colIdx, HSV_DEFAULT_VALUE, LINEARBLEND);
    colIdx += 1;
  }
}


CRGBPalette16 generateRandomPalette(uint8_t colCount) {
    CRGBPalette16 palette;  
    CRGB colorSet[colCount];
  
    for (int j = 0; j < colCount; j++) {
        random16_add_entropy( random(UCHAR_MAX));
        colorSet[j] = CHSV(random16(0, UCHAR_MAX), 255, HSV_DEFAULT_VALUE);
    }
  
    for( int i = 0; i < 16; ++i) {
        palette[i] = colorSet[i % colCount];
    }

    return palette;  
}
