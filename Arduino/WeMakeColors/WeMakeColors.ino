#include "FastLED.h"

//led
#define NUM_LEDS 9
#define DATA_PIN D1
#define GLOBAL_BRIGHTNESS 255

//presence
#define IN_PIN D2
#define MIN_TIME 6000 //delay between new color
volatile boolean newPresence = false;

//random color
byte COLOR_BITS = 5;
unsigned long MAX_C = pow(2, COLOR_BITS); // number of variaitons per each color
byte COLOR_MASK = MAX_C - 1;
unsigned long MAX_NUM =  pow(MAX_C, 3);

unsigned long rnd;
unsigned long loops;
byte r, g, b;


CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("We Make Colors");

  pinMode(IN_PIN, INPUT);

 
  FastLED.setBrightness(GLOBAL_BRIGHTNESS);
  FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS);
   for (int i = 0; i < NUM_LEDS ; i++) {
      leds[i] = CRGB(0,0,0);
    }
  FastLED.show();

  attachInterrupt(digitalPinToInterrupt(IN_PIN), presence_isr, RISING);

}

int calibrate( int rawVal) {
  float gammaL = 2.5;
  float inRange = MAX_C;
  float outRangeL = 255;

  float l;
  float lC;

  l = rawVal;
  lC = pow((l / inRange), gammaL) * outRangeL;
  return ((lC));

}

void loop() {

  loops++;
  if (newPresence) {
    newPresence = false;
    Serial.print("sec ");
    Serial.println(millis() / 1000);
    rnd_color();
    for (int i = 0; i < NUM_LEDS - 1; i++) {
      leds[i] = leds[i + 1];
    }
    leds[NUM_LEDS - 1] = CRGB(calibrate(r), calibrate(g),calibrate (b));
    FastLED.show();

  }
  delay(1);


}

void presence_isr() {
  static unsigned long lastMillis = 0;
  if (abs(millis() - lastMillis) > MIN_TIME) {
    newPresence = true;
    lastMillis = millis();
  }
}


void rnd_color() {
  unsigned long c = loops % MAX_NUM;

  r = c      & COLOR_MASK; // red
  g = c >> 4 & COLOR_MASK; //green
  b = c >> 8 & COLOR_MASK; //blue

  Serial.print ("spins ");
  Serial.println (loops / MAX_NUM);

  Serial.println("rgb ");
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  Serial.println("");

}



