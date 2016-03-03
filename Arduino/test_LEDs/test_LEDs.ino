#include "FastLED.h"

#define NUM_LEDS 9
#define DATA_PIN D1
#define GLOBAL_BRIGHTNESS 255

#define NUM_COLORS 9

CRGB leds[NUM_LEDS];
CRGB colors[NUM_COLORS];

void setup() {
  FastLED.setBrightness(GLOBAL_BRIGHTNESS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);

}

int calibrate( int rawVal) {
  float gammaL = 2.5;
  float outRangeL = 255;

  float inRange = 255;
  float l;
  float lC;

  l = rawVal;
  lC = pow((l / inRange), gammaL) * outRangeL;
  return ((lC));

}

void loop() {

  int r = calibrate(random(0, 220));
  int g = calibrate(random(0, 255));
  int b = calibrate(random(0, 255));

  for (int i = 0; i < NUM_COLORS - 1; i++) {
    colors[i] = colors[i + 1];
  }

  colors[NUM_COLORS - 1] = CRGB(r, g, b);
  for (int c = 0; c < NUM_COLORS; c++)
  {
    leds[c] = colors[c];
  }

  FastLED.show();

  int d = random(8000, 16000);
  delay(d);

}
