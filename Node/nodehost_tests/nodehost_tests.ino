#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <stdio.h>

#define LED_PIN 7
#define NUM_LEDS 144
#define UCHAR_MAX 255
#define FRAME_INTERVAL 16
#define HSV_DEFAULT_VALUE 128

CRGB leds[NUM_LEDS];
CRGB seedLeds[NUM_LEDS];
char logbuffer [80];
int logsize;
char *modes[4] = {"timing", "rainbow", "chase", "offset"};

unsigned long currTime, prevTime, startTime, cycleTime;
uint8_t hue;
uint8_t pos;
uint8_t offset;

/* Tests for NodeHost_MCU */

/* Setup  */
void setup() {

  /* Set pins */
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D4, OUTPUT);
  delay(500);
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.println("Activating LEDs");
  startTime = millis();
  cycleTime = millis();

  pos, hue, currTime, prevTime, startTime, offset = 0;

  /* Print test parameters */
  logsize = sprintf(logbuffer, "mode: %s, rate: %i", modes[2], FRAME_INTERVAL);
  Serial.println(logbuffer);

  /* Print test name */
  //Serial.println("testMosfetSwitch [SETUP]");
  //Serial.println("testMillis");
  //Serial.println("testLEDsRainbowDraw");
  //Serial.println("testLEDsSingleChaseDraw");
  Serial.println("testLEDsOffsetDraw");
  //Serial.println("testLEDsRainbowMaxRate");
  //Serial.println("testLEDsSingleChaseMaxRate");

  //* Setup tests */
  //testPower();

  generateRandomFrame(seedLeds);  
}

/* Loop */
void loop() {
  currTime = millis();

  //testMillis();
  //testLEDsRainbowDraw();
  //testLEDsSingleChaseDraw();
  testLEDsOffsetDraw();

  //testLEDsRainbowMaxRate();
  //testLEDsSingleChaseMaxRate();
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

/* Offset test */
void testLEDsOffsetDraw() {
  /* Frame condition */
  if (currTime - prevTime > FRAME_INTERVAL) {
    for(int i=0;i<NUM_LEDS;i++) {
      leds[i] = seedLeds[(i + offset)% NUM_LEDS];
    }
    offset++;
    FastLED.show();
    
    prevTime = currTime;
    
    /* Cycle condition */
    if (offset == UCHAR_MAX) {
      offset = 0;
      logsize = sprintf(logbuffer, "mode: %s, rate: %i, leds: %i, time: %i ms", modes[3], FRAME_INTERVAL, NUM_LEDS, millis() - cycleTime);
      Serial.println(logbuffer);
      cycleTime = millis();
      generateRandomFrame(leds);
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
    //frame[i].r = random(UCHAR_MAX);
    //frame[i].g = random(UCHAR_MAX);
    //frame[i].b = random(UCHAR_MAX);
    frame[i].setHSV(random(UCHAR_MAX), random(UCHAR_MAX), random(HSV_DEFAULT_VALUE));
  }
}
