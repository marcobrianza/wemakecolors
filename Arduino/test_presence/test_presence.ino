
//#define LED 13 
//#define IN 7 

#define LED 16
#define IN D2

void setup() {
  // put your setup code here, to run once:
pinMode(IN, INPUT);
pinMode(LED,OUTPUT);
Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
int p=digitalRead(IN);

digitalWrite(LED,p);
Serial.println(p); 
delay(100);


}
