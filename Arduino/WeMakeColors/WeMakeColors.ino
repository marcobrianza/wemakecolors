#include "FastLED.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
//const char* SSID = "...";
//const char* PASSWORD = "...";
#include "config.h"


const char* MQTT_SERVER = "net.marcobrianza.it";
const char* MQTT_TOPIC =   "/WeMakeColors/color";
const char* MQTT_ID = "ColorArduino";
const byte MSG_LEN = 3;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
byte myColor[MSG_LEN];
int value = 0;

//led
const int NUM_LEDS = 9;
#define LED_DATA_PIN D1
#define GLOBAL_BRIGHTNESS 255
#define LED_ORDER GRB //GRB=WS2812B  BRG=TM1809

//presence
#define IN_PIN D2
const unsigned int MIN_TIME = 60000; //delay between new color
volatile boolean newPresence = false;

//random color
unsigned long MAX_C = 16; // number of variaitons per each color
unsigned long MAX_CCC = MAX_C * MAX_C * MAX_C;
unsigned long MAX_NUM = MAX_CCC;

unsigned long rnd;
unsigned long countLoops;
byte r, g, b; //localy generated color
byte R, G, B; //color rrived from remote

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(600);
  Serial.println("");
  Serial.println("We Make Colors");

  FastLED.setBrightness(GLOBAL_BRIGHTNESS);
  FastLED.addLeds<WS2812B, LED_DATA_PIN, LED_ORDER>(leds, NUM_LEDS);
  memset(leds, 0, NUM_LEDS * 3);
  FastLED.show();

  pinMode(IN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IN_PIN), presence_isr, RISING);

  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}



void loop() {
  if (!client.connected())  reconnect();
  client.loop();
  
  countLoops++;

  if (newPresence) {
    newPresence = false;
    Serial.print("sec ");
    Serial.println(millis() / 1000);
    rnd_color();
    myColor[0] = r;  myColor[1] = g;  myColor[2] = b;
    client.publish(MQTT_TOPIC, myColor, 3);
  }
}

void rnd_color() {
  unsigned long c = countLoops % MAX_NUM;

  r = c    % MAX_C;               // red
  g = c / MAX_C  % MAX_C;         //green
  b = c / MAX_C  / MAX_C % MAX_C; //blue

  Serial.print ("countLoops ");   Serial.println (countLoops / MAX_NUM);
  Serial.println("rgb ");   Serial.println(r);   Serial.println(g);   Serial.println(b); 
}


void presence_isr() {
  static unsigned long lastMillis = 0;
  if (abs(millis() - lastMillis) > MIN_TIME) {
    newPresence = true;
    lastMillis = millis();
  }
}



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((byte)payload[i]);
    Serial.print(" ");
  }
  Serial.println();

  if (length == MSG_LEN) {
    R = payload[0];
    G = payload[1];
    B = payload[2];

    for (int i = 0; i < NUM_LEDS - 1; i++) {
      leds[i] = leds[i + 1];
    }
    leds[NUM_LEDS - 1] = CRGB(cal_lut(R), cal_lut(G), cal_lut (B));
    FastLED.show();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_ID)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


byte cal_lut(byte c) {
  if (c > 15) c = 15;
  byte lut[16] = {0, 11, 14, 17, 21, 27, 34, 42, 53, 66, 83, 104, 130, 163, 204, 255};
  return lut[c];
}


