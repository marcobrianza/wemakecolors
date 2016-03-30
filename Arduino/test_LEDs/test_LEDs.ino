#include "FastLED.h"

#define NUM_LEDS 9
#define DATA_PIN D1
#define GLOBAL_BRIGHTNESS 255
#define MAX_C 16

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.setBrightness(GLOBAL_BRIGHTNESS);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  memset(leds, 0, NUM_LEDS * 3);
  FastLED.show();

  randomSeed(analogRead(A0));
}


void loop() {
  int r = random(0, MAX_C);
  int g = random(0, MAX_C);
  int b = random(0, MAX_C);

  for (int i = 0; i < NUM_LEDS - 1; i++) {
    leds[i] = leds[i + 1];
  }

  byte R = cal_lut(r);
  byte G = cal_lut(g);
  byte B = cal_lut(b);

  leds[NUM_LEDS - 1] = CRGB(R, G, B);

  FastLED.show();
  delay(10000);

}

byte cal_lut(byte c) {
  if (c > 15) c = 15;
  byte lut[16] = {0, 11, 14, 17, 21, 27, 34, 42, 53, 66, 83, 104, 130, 163, 204, 255};
  return lut[c];
}




