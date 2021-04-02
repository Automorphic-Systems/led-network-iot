#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <stdio.h>

#define LED_PIN 7
#define NUM_LEDS 288
#define UCHAR_MAX 255
#define FRAME_INTERVAL 32
#define HSV_DEFAULT_VALUE 160

CRGB leds[NUM_LEDS];
uint8_t leds_h[NUM_LEDS];
CRGB seedLeds[NUM_LEDS];
CRGBPalette16 currentPalette;
char logbuffer [80];
int logsize;
char *modes[4] = {"timing", "rainbow", "chase", "flicker"};
 
unsigned long currTime, prevTime, startTime, cycleTime;
uint8_t hue;
uint8_t pos;
uint16_t offset;

static int heatmap[NUM_LEDS];
uint8_t seeds;

/* Tests for NodeHost_MCU */

/* Setup  */
void setup() {

  /* Set pins */
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D4, OUTPUT);
  delay(2500);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  startTime = millis();
  cycleTime = millis();

  pos, hue, currTime, prevTime, startTime, offset = 0;
  Serial.begin(115200);
  delay(100);
  Serial.flush();
  while ( Serial.available() ) Serial.read(); 
  
  /* Print test parameters */
  Serial.println("Activating LEDs");
  logsize = sprintf(logbuffer, "mode: %s, rate: %i", modes[2], FRAME_INTERVAL);
  Serial.println(logbuffer);

  /* Print test name */
  //Serial.println("testMosfetSwitch [SETUP]");
  //Serial.println("testMillis");
  //Serial.println("testLEDsRainbowDraw");
  //Serial.println("testLEDsSingleChaseDraw");
  //Serial.println("testLEDsFullChaseDraw");
  //Serial.println("testLEDsRainbowMaxRate");
  //Serial.println("testLEDsSingleChaseMaxRate");
  Serial.println("testFlickeringDraw");

  //* Setup tests */
  //testMosfetSwitch();

  generateRandomPaletteFrame(seedLeds);  
}

/* Loop */
void loop() {
  currTime = millis();

  //testMillis();
  //testLEDsRainbowDraw();
  //testLEDsSingleChaseDraw();
  //testLEDsFullChaseDraw();

  //testLEDsRainbowMaxRate();
  //testLEDsSingleChaseMaxRate();
  //testRandomPalettes();
  testFlickeringDraw();
}

/* Test LED load being switched on/off via MOSFET */
void testMosfetSwitch() {
  Serial.println("Activating power load.");
  int powCnt = 0;

  Serial.println("Drawing LEDs.");
  FastLED.showColor(CHSV(128, 255, HSV_DEFAULT_VALUE));

  for (powCnt = 0; powCnt < 5; powCnt++) {
    Serial.println("Turning on.");
    digitalWrite(D4, LOW);
    delay(2000);

    Serial.println("Turning off.");
    digitalWrite(D4, HIGH); /* LEDs should go dark */
    delay(2000);
  }

  Serial.println("Turning on.");
  digitalWrite(D4, LOW);
}


/* Test millis call */
void testMillis() {

  if (currTime  - prevTime > 1000) {
    Serial.println("Incrementing second");
    prevTime = currTime;
  }

}


/* Rainbow mode test */
void testLEDsRainbowDraw() {
  /* Frame condition */
  if (currTime - prevTime > FRAME_INTERVAL) {
    FastLED.showColor(CHSV(hue++, 255, HSV_DEFAULT_VALUE));
    prevTime = currTime;
  }

  /* Cycle condition */
  if (hue >= UCHAR_MAX) {
    hue = 0;

    logsize = sprintf(logbuffer, "mode: %s, rate: %i, leds: %i, time: %i ms", modes[1], FRAME_INTERVAL, NUM_LEDS, millis() - cycleTime);
    Serial.println(logbuffer);
    cycleTime = millis();
  }
}


/* Chase mode test */
void testLEDsSingleChaseDraw() {
  /* Frame condition */
  if (currTime - prevTime > FRAME_INTERVAL) {
    if (pos < NUM_LEDS - 1) {
      leds[pos] = CHSV(hue, 0, 0);
      leds[pos + 1] = CHSV(hue, 255, 255);
      hue++;
      pos++;
    } else {
      leds[pos] = CHSV(hue, 0, 0);
      pos = 0;
      leds[pos] = CHSV(hue, 255, 255);
    }
    FastLED.show();

    prevTime = currTime;

    /* Cycle condition */
    if (pos == NUM_LEDS - 1 ) {
      logsize = sprintf(logbuffer, "mode: %s, rate: %i, leds: %i, time: %i ms", modes[2], FRAME_INTERVAL, NUM_LEDS, millis() - cycleTime);
      Serial.println(logbuffer);
      cycleTime = millis();
    }
  }
}

/* Full chase mode test, with palettes */
void testLEDsFullChaseDraw() {
  /* Frame condition */
  if (currTime - prevTime > FRAME_INTERVAL) {
    for(int i=0;i<NUM_LEDS;i++) {
      leds[i] = seedLeds[(i + offset)% NUM_LEDS];
    }
    offset--;
    FastLED.show();
    
    prevTime = currTime;
    
    /* Cycle condition - flip offset to change chase direction*/
    if (offset == 0) {
      offset = UCHAR_MAX;
      logsize = sprintf(logbuffer, "mode: %s, rate: %i, leds: %i, time: %i ms", modes[2], FRAME_INTERVAL, NUM_LEDS, millis() - cycleTime);
      Serial.println(logbuffer);
      cycleTime = millis();
      generateRandomPaletteFrame(seedLeds);
    } 
  }
}

/* Flickering mode test, with palettes */
void testFlickeringDraw() {
  /* Frame condition */
  if (currTime - prevTime > FRAME_INTERVAL) {
    randomSeed(analogRead(0));
    seeds = random16(10, NUM_LEDS - 10);

    for ( int i = 0; i < NUM_LEDS; i++) {
        heatmap[i] = qsub8( heatmap[i], 6);
    }
    
    for ( int j = 0 ; j < seeds ; j++) {
     if (random8() < 32) {
      //again, we have to mix things up so the same locations don't always light up
      randomSeed(analogRead(0));
      heatmap[random16(NUM_LEDS)] = random16(48, 160);
     }
    }

    for ( int k = 0 ; k < NUM_LEDS ; k++ ) {
      if (heatmap[k] > 0 && random8() < 100) {
        heatmap[k] = qadd8(heatmap[k] , 5);
      }
    }

    for ( int j = 0; j < NUM_LEDS; j++)
    {
      leds[j].setHSV(leds_h[j], 255, heatmap[j]);
    }
    offset++;

    FastLED.show();    
    prevTime = currTime;
    
    if (offset == 1024) {
      offset = 0;
      seeds = random16(NUM_LEDS - 20 , NUM_LEDS);
      FastLED.clear();

      for ( int i = 0 ; i < seeds ; i++) 
      {
         int pos = random16(NUM_LEDS);
         randomSeed(analogRead(0));
         heatmap[pos] = random8(50, 255);
      }

      currentPalette = generateRandomPalette(3);
      for ( int j = 0; j < NUM_LEDS; j++)
      {
        CRGB currLed = ColorFromPalette( currentPalette, random8(0, 15), heatmap[j], LINEARBLEND);
        leds[j] = currLed;
        leds_h[j] = rgb2hsv_approximate( currLed ).h;
      }
      
      FastLED.show();
      logsize = sprintf(logbuffer, "mode: %s, rate: %i, leds: %i, time: %i ms", modes[3], FRAME_INTERVAL, NUM_LEDS, millis() - cycleTime);
      Serial.println(logbuffer);
      cycleTime = millis();
    } 
  }
}

/* MAX RATE TESTS - These will very likely cause the hardware to crash and reset */

/* Rainbow test, max rate */
void testLEDsRainbowMaxRate() {
  hue = 0;
  unsigned long startTime = millis();

  while (hue < UCHAR_MAX) {
    FastLED.showColor(CHSV(hue++, 255, HSV_DEFAULT_VALUE));
  }

  logsize = sprintf(logbuffer, "mode: %s, rate: MAX, leds: %i, time: %i ms", modes[1], NUM_LEDS, millis() - startTime);
  Serial.println(logbuffer);
}


/* Chase test, max rate */
void testLEDsSingleChaseMaxRate() {
  if (pos < NUM_LEDS - 1) {
    leds[pos] = CHSV(hue, 0, 0);
    leds[pos + 1] = CHSV(hue, 255, 255);
    pos++;
  } else {
    hue++;
    leds[pos] = CHSV(hue, 0, 0);
    pos = 0;
    leds[pos] = CHSV(hue, 255, 255);
  }

  FastLED.show();

  if (pos == NUM_LEDS - 1 ) {
    hue = 0;
    logsize = sprintf(logbuffer, "mode: %s, rate: MAX, leds: %i, time: %i ms", modes[2], NUM_LEDS, millis() - cycleTime);
    Serial.println(logbuffer);
    cycleTime = millis();
  }
}


/* Helper Methods */
void generateRandomFrame(CRGB frame[]) {  
  randomSeed(analogRead(0));
  
  for (int i = 0; i < NUM_LEDS; i++) {
    frame[i].setHSV(random(UCHAR_MAX), random(UCHAR_MAX), random(HSV_DEFAULT_VALUE));
  }
}

void generateRandomPaletteFrame(CRGB frame[]) {
  
  currentPalette = generateRandomPalette(3);
  
  uint8_t colIdx;
  colIdx = 1;
  for (int i = 0; i < NUM_LEDS; i++) {
    frame[i] = ColorFromPalette( currentPalette, colIdx, HSV_DEFAULT_VALUE, LINEARBLEND);
    colIdx += 1;
  }
}


CRGBPalette16 generateRandomPalette(uint8_t colCount) {
  CRGBPalette16 palette;
  
  CRGB colorSet[colCount];
  randomSeed(analogRead(0));
  
  for (int j = 0; j < colCount; j++) {
    colorSet[j] = CHSV(random8(), 255, HSV_DEFAULT_VALUE);
  }
  
  for( int i = 0; i < 16; ++i) {
        palette[i] = colorSet[i % colCount];
  }

  return palette;  
}


CHSV TwinkleColor( int temperature)
{
  CHSV heatcolor;
  heatcolor.hue = 60;
  heatcolor.saturation = 255;
  heatcolor.value = temperature;
  return heatcolor;
}
