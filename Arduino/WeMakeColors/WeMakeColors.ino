#include "FastLED.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
//const char* SSID = "...";
//const char* PASSWORD = "...";

const char* SSID = "PucciHome24G";
const char* PASSWORD = "Grandebellezza3!";

const char* MQTT_SERVER = "net.marcobrianza.it";
const char* MQTT_TOPIC =   "/WeMakeColors/color";
const char* MQTT_ID = "ColorArduino";
const byte MSG_LEN = 3;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
byte myColor[MSG_LEN];
//byte newColor[MSG_LEN];
int value = 0;

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
byte r, g, b; //localy generated color
byte R, G, B; //color rrived from remote

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("We Make Colors");
  
  FastLED.setBrightness(GLOBAL_BRIGHTNESS);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  for (int i = 0; i < NUM_LEDS ; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();

  pinMode(IN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IN_PIN), presence_isr, RISING);

  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  loops++;

  if (newPresence) {
    newPresence = false;
    Serial.print("sec ");
    Serial.println(millis() / 1000);
    rnd_color();
    myColor[0] = r;
    myColor[1] = g;
    myColor[2] = b;

    client.publish(MQTT_TOPIC, myColor, 3);
  }
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
    leds[NUM_LEDS - 1] = CRGB(calibrate(R), calibrate(G), calibrate (B));
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


int calibrate( int rawVal) {
  float gammaL = 2.5;
  float inRange = MAX_C-1;
  float outRangeL = 255;

  float l;
  float lC;

  l = rawVal;
  lC = pow((l / inRange), gammaL) * outRangeL;
  return ((lC));

}
