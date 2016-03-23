
//#define LED 13
//#define IN 7

#define LED 16
#define IN D2
#define MIN_TIME 60000

volatile boolean newPresence=false;

void setup() {
  // put your setup code here, to run once:
  pinMode(IN, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  attachInterrupt(digitalPinToInterrupt(IN), presence_isr, RISING);
}

void loop() {

  if (newPresence){
    newPresence=false;
  int p = digitalRead(IN);
    digitalWrite(LED, LOW);
    Serial.println(millis()/1000);
    
  }else digitalWrite(LED, HIGH);
  
  delay(1);

}

void presence_isr() {
static unsigned long lastMillis = 999999;
  if (abs(millis() - lastMillis) > MIN_TIME) {
    newPresence=true;
    lastMillis = millis();
  } 
}



