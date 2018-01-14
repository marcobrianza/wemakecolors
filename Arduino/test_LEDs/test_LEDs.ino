// test software for We Make Colors
// Marco Brianza - April 9th 2016
//
// test LEDs



#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#if defined(ESP8266)
#define LED_DATA_PIN 5 //D1 is GPIO5
//#define FASTLED_ALLOW_INTERRUPTS 0
#endif

#if defined(ARDUINO_SAMD_MKR1000)
#define LED_DATA_PIN 5
#endif


#define NUM_LEDS 9
#define GLOBAL_BRIGHTNESS 255
#define MAX_C 16

CRGB leds[NUM_LEDS];

#include <ESP8266WiFi.h>

void setup() {
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  delay(2000);
  Serial.println("\n test LEDs");

  FastLED.setBrightness(GLOBAL_BRIGHTNESS);



  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  //memset(leds, 0, NUM_LEDS * 3);


  randomSeed(analogRead(A0));

  for (int i = 0; i < NUM_LEDS - 1; i++) {
    leds[i] = newRndColor();
  }

  FastLED.show();
}


void loop() {

  for (int i = 0; i < NUM_LEDS - 1; i++) {
    leds[i] = leds[i + 1];
  }

  leds[NUM_LEDS - 1] = newRndColor();
  FastLED.show();

  delay(6000);
}

byte cal_lut(byte c) {
  if (c > 15) c = 15;
  byte lut[16] = {0, 11, 14, 17, 21, 27, 34, 42, 53, 66, 83, 104, 130, 163, 204, 255};
  return lut[c];
}

CRGB newRndColor() {
  int r = random(0, MAX_C);
  int g = random(0, MAX_C);
  int b = random(0, MAX_C);

  byte R = cal_lut(r);
  byte G = cal_lut(g);
  byte B = cal_lut(b);

  Serial.print(R); Serial.print(" "); Serial.print(G); Serial.print(" "); Serial.println(G);

  return CRGB(R, G, B);
}



