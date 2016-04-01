
//#define LED 13 
//#define IN 7 

#define LED 16
#define IN D2

void setup() {
pinMode(IN, INPUT);
pinMode(LED,OUTPUT);
Serial.begin(115200);
Serial.println("test presence");
delay (2000);

}

void loop() {
int p=digitalRead(IN);

digitalWrite(LED,p);
Serial.println(p); 
delay(100);


}
