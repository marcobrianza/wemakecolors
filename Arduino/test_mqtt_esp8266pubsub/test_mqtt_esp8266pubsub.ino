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

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* SSID = "...";
const char* PASSWORD = "...";

//const char* MQTT_SERVER = "test.mosquitto.org";
const char* MQTT_SERVER = "net.marcobrianza.it";
//const char* MQTT_SERVER = "192.168.1.128";
const char* MQTT_TOPIC =   "/WeMakeColors/color";
const char* MQTT_ID = "ColorArduino";

const byte MSG_LEN = 3;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
byte myColor[MSG_LEN];
byte newColor[MSG_LEN];
int value = 0;



void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);

  //test color = teal
  myColor[0] = 1;
  myColor[1] = 130;
  myColor[2] = 131;
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
    newColor[0] = payload[0];
    newColor[1] = payload[1];
    newColor[2] = payload[2];
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
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    Serial.print("Publish message: ");
    for (int i = 0; i < MSG_LEN; i++) {
      Serial.print((byte)myColor[i]);
      Serial.print(" ");
    }
    Serial.println();
    client.publish(MQTT_TOPIC, myColor, 3);
  }
}
