// test software for We Make Colors
// Marco Brianza - April 9th 2016
//
// test presence polling



#if defined(ESP8266)
#define IN_PIN D2
#endif

#if defined(ARDUINO_SAMD_MKR1000)
#define IN_PIN 4
#endif



void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n test presence polling");
  delay (2000);

  pinMode(IN_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);


}

void loop() {
  int p = digitalRead(IN_PIN);

  digitalWrite(LED_BUILTIN, !p); //LED is active LOW on NodeMCU Amica
  Serial.println(p);
  delay(100);


}
