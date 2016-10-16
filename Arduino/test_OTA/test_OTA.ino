#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

// Update these with values suitable for your network.
const char* SSID = "...";
const char* PASSWORD = "...";

const char *AP_SSID = "thingAP";
const char *AP_PASSWORD = "password";

const char* OTA_PASSWORD = "12345678";

boolean led = LOW;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\ntest OTA");

  //connect_wifi();
  startAP();

  setupOTA();

  pinMode(LED_BUILTIN, OUTPUT);
}


void loop() {

  ArduinoOTA.handle();
  delay(1000);
  digitalWrite(LED_BUILTIN, led);
  Serial.println(led);
  led = !led;

}



void setupOTA() {
  char* THING_ID = "OTA-11:22:33:44:55:66"; // (mac will be changed automatically)
  byte ma[6];  WiFi.macAddress(ma);
  sprintf(THING_ID, "OTA-%02X:%02X:%02X:%02X:%02X:%02X", ma[0], ma[1], ma[2], ma[3], ma[4], ma[5]);
  Serial.println(THING_ID);

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


///----- Wi-Fi-----------------------------


void startAP() {
  IPAddress ip(1, 1, 1, 1);
  IPAddress nm(255, 255, 255, 0);
  WiFi.softAPConfig(ip, ip, nm);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.println("AP started");
}

void connect_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.print("Connecting to "); Serial.println(SSID);
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


