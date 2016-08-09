// test software for We Make Colors
// Marco Brianza - April 9th 2016
//
// test button



#define CONFIG_PIN 0


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n test button");

  pinMode(CONFIG_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  int p = digitalRead(CONFIG_PIN);

  digitalWrite(LED_BUILTIN, !p); //LED is active LOW on NodeMCU Amica
  Serial.println(p);
  delay(100);


}
