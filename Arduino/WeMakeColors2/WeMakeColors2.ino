

#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include <PubSubClient.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#define IN_PIN D2
#define LED_DATA_PIN 5 // D2 is GPIO5
#define CONFIG_PIN 0  // set to GND at startup to force config using builtin button

const char* OTA_PASSWORD = "12345678";

char* THING_ID = "WeMakeColors-11:22:33:44:55:66"; // THING_ID (will be changed automatically on ESP8266)

const char* MQTT_SERVER = "wmc.marcobrianza.it";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC =   "/WeMakeColors/color";

const byte MSG_LEN = 3;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

long lastMsg = 0;
byte myColor[MSG_LEN];
int value = 0;

boolean led = LOW;

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
  leds[0] = CRGB(64, 64, 64);
  FastLED.show();

  pinMode(IN_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(CONFIG_PIN, INPUT_PULLUP);


  int i = digitalPinToInterrupt(IN_PIN);
  attachInterrupt(i, presence_isr, RISING);

  byte ma[6];
  WiFi.macAddress(ma);

  sprintf(THING_ID, "WeMakeColors-%02X:%02X:%02X:%02X:%02X:%02X", ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);

  Serial.print("THING_ID: ");
  Serial.println(THING_ID);

  connect_wifi_or_AP(false);

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqtt_callback);

  setupOTA();
}



void loop() {
  if (!mqttClient.connected())  reconnectMQTT();
  mqttClient.loop();

  countLoops++;

  if (newPresence) {
    newPresence = false;
    Serial.print("sec "); Serial.println(millis() / 1000);
    rnd_color();
    myColor[0] = r;  myColor[1] = g;  myColor[2] = b;
    mqttClient.publish(MQTT_TOPIC, myColor, 3);
  }

  ArduinoOTA.handle();
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
  static unsigned long lastMillis = MIN_TIME + 1; // this initialization enables to generate a new color at startup
  if (abs(millis() - lastMillis) > MIN_TIME) {
    newPresence = true;
    lastMillis = millis();
  }
}


void connect_wifi_or_AP(bool force_config) {
  digitalWrite(LED_BUILTIN, LOW);

  //  WiFi.disconnect();
  //  delay(100);

  WiFiManager wifiManager;
  wifiManager.setDebugOutput(true);
  wifiManager.setAPStaticIPConfig(IPAddress(1, 1, 1, 1), IPAddress(1, 1, 1, 1), IPAddress(255, 255, 255, 0));

  if ((digitalRead(CONFIG_PIN) == LOW) || (force_config == true)) { //config must be done
    wifiManager.setConfigPortalTimeout(0);
    wifiManager.resetSettings(); //reset saved settings
    wifiManager.startConfigPortal(THING_ID);
  } else
  {
    wifiManager.setConfigPortalTimeout(600);
    wifiManager.autoConnect(THING_ID);
  }

  WiFi.mode(WIFI_STA);

  boolean led = false;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, led);
    led = !led;
  }

  //if you get here you have connected to the WiFi
  Serial.print("connected to network ");
  Serial.println(WiFi.SSID());

  digitalWrite(LED_BUILTIN, HIGH);
  leds[0] = CRGB(0, 64, 0);
  FastLED.show();
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

void reconnectMQTT() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("\nAttempting MQTT connection...");
    // Attempt to connect
     wifiClient = WiFiClient(); // workaround to fix reconnection
    if (mqttClient.connect(THING_ID)) {
      digitalWrite(LED_BUILTIN, HIGH);
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

void setupOTA() {

  ArduinoOTA.setPort(8266); // Port defaults to 8266
  ArduinoOTA.setHostname(THING_ID);   // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setPassword((const char *) OTA_PASSWORD);   // No authentication by default

  ArduinoOTA.onStart([]() {
    Serial.println("\nStart OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd OTA");
    delay(100);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}


