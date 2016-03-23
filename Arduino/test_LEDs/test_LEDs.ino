#include "FastLED.h"

#define NUM_LEDS 9
#define DATA_PIN D1
#define GLOBAL_BRIGHTNESS 255


CRGB leds[NUM_LEDS];

void setup() {
  FastLED.setBrightness(GLOBAL_BRIGHTNESS);
  FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);
  randomSeed(analogRead(A0));

}

int calibrate( int rawVal) {
  float gammaL = 2.5;
  float inRange = 16;
  float outRangeL = 255;


  float l;
  float lC;

  l = rawVal;
  lC = pow((l / inRange), gammaL) * outRangeL;
  return ((lC));

}

void loop() {

  int r = calibrate(random(0, 15));
  int g = calibrate(random(0, 15));
  int b = calibrate(random(0, 15));

  for (int i = 0; i < NUM_LEDS - 1; i++) {
    leds[i] = leds[i + 1];
  }
  leds[NUM_LEDS - 1] = CRGB(r, g, b);
  
  FastLED.show();

  int d = random(8000, 16000);
  delay(d);

}
