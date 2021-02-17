#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>

#define LED_PIN 7
#define NUM_LEDS 60
#define UCHAR_MAX 255
#define FRAME_INTERVAL 32

CRGB leds[NUM_LEDS];
unsigned long currTime, prevTime, startTime, cycleTime; 
uint8_t hue;  
uint8_t pos;

/* Tests for NodeHost_MCU */

/* Setup  */
void setup() {
  
   pinMode(LED_BUILTIN, OUTPUT);
   delay(2500);
   Serial.begin(115200);
   FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
   Serial.println("Activating LEDs"); 
   startTime = millis();   
   cycleTime = millis();   

    pos = 0;
    hue = 0;
    currTime = 0;
    prevTime = 0;
    startTime = 0; 

   // * Specify which test to run */
   //Serial.println("testMillis: Millis test started");
   //Serial.println("testLEDsRainbowMaxRate: LED rainbow test started, maximum rate");
   Serial.println("testLEDsRainbowDraw rainbow test, single draw per frame interval");
   //Serial.println("testLEDsSingleChaseMaxRate chase test, maximum rate");
}

/* Loop */
void loop() {
   currTime = millis();  
    
   //testMillis();   
   //testLEDsRainbowMaxRate();
   testLEDsRainbowDraw();
   //testLEDsSingleChaseMaxRate();
} 


/* Test to see if millis is creating a crash */
void testMillis() {
  
  if (currTime  - prevTime > 1000) {
     Serial.println("Incrementing second");
     prevTime = currTime;           
  }
 
}

/* Test LED strip - rainbow test */
void testLEDsRainbowMaxRate() {
    hue = 0;  
    unsigned long startTime = millis();     
    
    while(hue < UCHAR_MAX) {                 
        FastLED.showColor(CHSV(hue++, 255, 255)); 
    }
     
    FastLED.clear();
    FastLED.show();
    Serial.print(NUM_LEDS);
    Serial.print(" LEDs cycled in: ");
    Serial.println(millis()-startTime);  
}

/* Test LED strip rainbow test - 1 full draw per frame interval */
void testLEDsRainbowDraw() {
    if (currTime - prevTime > FRAME_INTERVAL) {               
        FastLED.showColor(CHSV(hue++, 255, 255)); 
        prevTime = currTime;
    }

    if (hue >= UCHAR_MAX) {
        hue = 0;
        FastLED.clear();
        FastLED.show();
        Serial.print(NUM_LEDS);
        Serial.print(" LEDs cycled in: ");
        Serial.println(millis()-cycleTime); 
        cycleTime = millis(); 
    }
}

/* Test LED strip - chase, maximum rate, full draw */
void testLEDsSingleChaseMaxRate() {
    if (pos<NUM_LEDS-1) {
      leds[pos]=CHSV(hue,0,0);
      leds[pos+1]=CHSV(hue,255,255);
    } else {
      hue++;
      leds[pos]=CHSV(hue,0,0);
      pos = 0;
      leds[pos]=CHSV(hue,255,255);
    }
    
    pos++;
    FastLED.show();
    
    if (hue >= UCHAR_MAX) {
        hue = 0;
        FastLED.clear();
        FastLED.show();
        Serial.print(NUM_LEDS);
        Serial.print(" LEDs cycled in: ");
        Serial.println(millis()-cycleTime); 
        cycleTime = millis(); 
    }
}

/* Test LED strip - single chase */
void testLEDsSingleChaseDraw() {

  
}

/* Test FastLED.ShowColor() */
/* Show colors as fast as possible */
/* Minimize delay */
void testFastLEDShowColor() {


}

/* Test FastLED.ShowColor() */
/* Show colors as fast as possible */
/* Minimize delay */
void testFastLEDFill() {

  
}

/* Create frames of random values */
/* Push frames as fast as possible */
/* Minimize delay */
void testFastLEDShowColorRandom() {


}
