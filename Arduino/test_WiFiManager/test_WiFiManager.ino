/**
test WiFiManager
*/

#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#define CONFIG_PIN 0  //set to GND at startup to force config

char* THING_ID = "WFM-11:22:33:44:55:66"; // (mac will be changed automatically)

WiFiClient wifiClient;

boolean led=LOW;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(CONFIG_PIN, INPUT_PULLUP);

  byte ma[6];  WiFi.macAddress(ma);
  sprintf(THING_ID, "WFM-%02X:%02X:%02X:%02X:%02X:%02X", ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);

  Serial.begin(115200);
  delay(2000);
  Serial.println();

  Serial.println(THING_ID);

  connect_wifi_or_AP(false);
  
}


void loop() {

  delay(1000);
  digitalWrite(LED_BUILTIN,led);
  Serial.println(led);
  led=!led;
}


//---WifiManager---------------------

void connect_wifi_or_AP(bool force_config) {
  digitalWrite(LED_BUILTIN, LOW);

//  WiFi.disconnect();
//  delay(100);

  WiFiManager wifiManager;
  wifiManager.setDebugOutput(true);
  wifiManager.setAPStaticIPConfig(IPAddress(1, 1, 1, 1), IPAddress(1, 1, 1, 1), IPAddress(255, 255, 255, 0));

  if ((digitalRead(CONFIG_PIN) == LOW) || (force_config == true)) { //config must be done
    wifiManager.setConfigPortalTimeout(0);
    //wifiManager.resetSettings(); //reset saved settings
    wifiManager.startConfigPortal(THING_ID);
  } else
  {
    wifiManager.setConfigPortalTimeout(60);
    wifiManager.autoConnect(THING_ID);
  }

  boolean led = false;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, led);
    led = !led;
  }

  //if you get here you have connected to the WiFi
  Serial.print("connected to network: ");
  Serial.println(WiFi.SSID());


  digitalWrite(LED_BUILTIN, HIGH);
  WiFi.mode(WIFI_STA);

}



