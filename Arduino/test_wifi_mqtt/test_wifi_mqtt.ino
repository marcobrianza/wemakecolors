/*
  Basic ESP8266 MQTT example modified for wemakecolors

  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.

  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
  http://pubsubclient.knolleary.net/api.html

*/

// test software for We Make Colors
// Marco Brianza - April 9th 2016
//
// test WiFi MQTT

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#if defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#endif

#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* SSID = "...";
const char* PASSWORD = "...";


char* MQTT_ID = "WMC_11:22:33:44:55:66"; // MQTT_ID (will be changed automatically on ESP8266)

const char* MQTT_SERVER = "net.marcobrianza.it";
//const char* MQTT_SERVER = "test.mosquitto.org";

const int MQTT_PORT = 1883;
const char* MQTT_TOPIC =   "/WeMakeColors/color_test";

const byte MSG_LEN = 3;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
long lastMsg = 0;
byte myColor[MSG_LEN];
byte newColor[MSG_LEN];
int value = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n test WiFi MQTT");

  connect_wifi();

  byte ma[6];
  WiFi.macAddress(ma);

  //  Serial.print("MAC: ");
  //  Serial.print(ma[0], HEX);
  //  Serial.print(":");
  //  Serial.print(ma[1], HEX);
  //  Serial.print(":");
  //  Serial.print(ma[2], HEX);
  //  Serial.print(":");
  //  Serial.print(ma[3], HEX);
  //  Serial.print(":");
  //  Serial.print(ma[4], HEX);
  //  Serial.print(":");
  //  Serial.println(ma[5], HEX);
#if defined(ESP8266)
  sprintf(MQTT_ID, "WMC_%02X:%02X:%02X:%02X:%02X:%02X", ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);
#endif

  Serial.print("MQTT_ID: ");
  Serial.println(MQTT_ID);


  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqtt_callback);

  //test color = teal
  myColor[0] = 1;
  myColor[1] = 12;
  myColor[2] = 12;
}

void connect_wifi() {
 WiFi.disconnect();
  delay(100);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to "); Serial.println(SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

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
    newColor[0] = payload[0];
    newColor[1] = payload[1];
    newColor[2] = payload[2];
  }


}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("\nAttempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(MQTT_ID)) {
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
void loop() {

  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    Serial.print("Publish message: ");
    for (int i = 0; i < MSG_LEN; i++) {
      Serial.print((byte)myColor[i]);
      Serial.print(" ");
    }
    Serial.println();
    mqttClient.publish(MQTT_TOPIC, myColor, 3);
  }
}
