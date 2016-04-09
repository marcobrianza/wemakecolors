// test software for We Make Colors
// Marco Brianza - April 9th 2016
//
// test presence interrupt


#define IN_PIN D2
#define MIN_TIME 6000 //delay between new color

volatile boolean newPresence = false;


void setup() {
  // put your setup code here, to run once:
  pinMode(IN_PIN, INPUT);
  
  Serial.begin(115200);
  delay(600);
  Serial.println("");
  Serial.println("test presence interrupt");
  delay (2000);
  
  attachInterrupt(digitalPinToInterrupt(IN_PIN), presence_isr, RISING);

}

void loop() {

  if (newPresence) {
    newPresence = false;
    Serial.print("sec ");
    Serial.println(millis() / 1000);
  }
  delay(1);
}

void presence_isr() {
  static unsigned long lastMillis = 0;
  if (abs(millis() - lastMillis) > MIN_TIME) {
    newPresence = true;
    lastMillis = millis();
  }
}



