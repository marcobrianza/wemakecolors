
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#define IN_PIN D2
#define LED_DATA_PIN 1
#endif

#if defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#define IN_PIN 4
#define LED_DATA_PIN 5
#endif

#include <PubSubClient.h>
#include "FastLED.h"

// Update these with values suitable for your network and device.
const char* SSID = "...";
const char* PASSWORD = "...";

char* MQTT_ID = "WMC_11:22:33:44:55:66"; // MQTT_ID (will be changed automatically on ESP8266)

const char* MQTT_SERVER = "net.marcobrianza.it";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC =   "/WeMakeColors/color";

const byte MSG_LEN = 3;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

long lastMsg = 0;
byte myColor[MSG_LEN];
int value = 0;

//LEDs
const int NUM_LEDS = 9;
#define GLOBAL_BRIGHTNESS 255
#define LED_ORDER GRB //GRB=WS2812B  BRG=TM1809

//presence
//#define IN_PIN D2 //defined in core selection
const unsigned int MIN_TIME = 60000; //delay between new color
volatile boolean newPresence = false;

//random color
unsigned long MAX_C = 16; // number of variaitons per each color
unsigned long MAX_CCC = MAX_C * MAX_C * MAX_C;
unsigned long MAX_NUM = MAX_CCC;

unsigned long rnd;
unsigned long countLoops;
byte r, g, b; //locally generated color
byte R, G, B; //color received from remote

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n WeMakeColors");

  FastLED.setBrightness(GLOBAL_BRIGHTNESS);
  FastLED.addLeds<WS2812B, LED_DATA_PIN, LED_ORDER>(leds, NUM_LEDS);
  memset(leds, 0, NUM_LEDS * 3);
  FastLED.show();

  pinMode(IN_PIN, INPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  
  int i;
#if defined(ESP8266)
  i = digitalPinToInterrupt(IN_PIN);
#endif

#if defined(ARDUINO_ARCH_SAMD)
  i = IN_PIN;
#endif

  attachInterrupt(i, presence_isr, RISING);

  setup_wifi();

  byte ma[6];
  WiFi.macAddress(ma);

#if defined(ESP8266)
  sprintf(MQTT_ID, "WMC_%02X:%02X:%02X:%02X:%02X:%02X", ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);
#endif

  Serial.print("MQTT_ID: ");
  Serial.println(MQTT_ID);

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqtt_callback);
}



void loop() {
  if (!mqttClient.connected())  reconnect();
  mqttClient.loop();

  countLoops++;

  if (newPresence) {
    newPresence = false;
    Serial.print("sec "); Serial.println(millis() / 1000);
    rnd_color();
    myColor[0] = r;  myColor[1] = g;  myColor[2] = b;
    mqttClient.publish(MQTT_TOPIC, myColor, 3);
  }
}

void rnd_color() {
  unsigned long c = countLoops % MAX_NUM;

  r = c    % MAX_C;               // red
  g = c / MAX_C  % MAX_C;         //green
  b = c / MAX_C  / MAX_C % MAX_C; //blue

  Serial.print ("countLoops ");   Serial.println (countLoops / MAX_NUM);
  Serial.print("rgb ");   Serial.print(r); Serial.print(" ");   Serial.print(g); Serial.print(" ");  Serial.println(b);
}


void presence_isr() {
  static unsigned long lastMillis = MIN_TIME+1; // this initialization enables to generate a new color at startup
  if (abs(millis() - lastMillis) > MIN_TIME) {
    newPresence = true;
    lastMillis = millis();
  }
}



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("\nConnecting to network: "); Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

 boolean led=false;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN,led);
    led=!led;
  }
  digitalWrite(LED_BUILTIN,HIGH);
  

  IPAddress ip = WiFi.localIP();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(ip);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
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
  while (!mqttClient.connected()) {
    digitalWrite(LED_BUILTIN,LOW);
    Serial.print("\nAttempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(MQTT_ID)) {
      digitalWrite(LED_BUILTIN,HIGH);
      Serial.println("connected\n");
      // ... and resubscribe
      mqttClient.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
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


