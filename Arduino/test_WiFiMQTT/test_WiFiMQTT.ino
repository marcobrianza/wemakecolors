// This example uses an nodeMCU Amica 1.0 ESP8266
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// based on the example by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ESP8266WiFi.h>
#include <MQTTClient.h>

const char *ssid = "mySSID";
const char *pass = "myPASS";



//const char *broker = "broker.shiftr.io";
//const char *username = "try";
//const char *password = "try";
//const char *clientId = "arduinoColor";

//const char *broker = "test.mosquitto.org";
const char *broker = "broker.hivemq.com";
const char *username = "";
const char *password = "";
const char *clientId = "";



WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

#define MSG_BYTES 3
byte color[MSG_BYTES];
byte newColor[MSG_BYTES];

void connect(); // <- predefine connect() for setup()

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  client.begin(broker, net);
 
  color[0] = 1;
  color[1] = 128;
  color[2] = 129;
    
  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(clientId, username, password)) {
    Serial.print(".");
  }

  Serial.println("\nconnected!");

  client.subscribe("/color");
  // client.unsubscribe("/example");
}

void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    client.publish("/color", (char*)color, sizeof(color));
  }
}

void messageReceived(String topic, String payload, char * mBytes, unsigned int mLength) {


  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.print(" - ");
  Serial.print(payload.length());
  Serial.print(" - ");

  if( mLength == MSG_BYTES) {
    memmove(newColor, mBytes, MSG_BYTES);

    Serial.print(byte(newColor[0]));  Serial.print(",");
    Serial.print(byte(newColor[1]));  Serial.print(",");
    Serial.print(byte(newColor[2]));  Serial.print(",");

  }
  Serial.println();
}
