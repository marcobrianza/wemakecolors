// test software for We Make Colors
// Marco Brianza - April 9th 2016
//
// test presence polling

#define IN D2

void setup() {
pinMode(IN, INPUT);
pinMode(LED_BUILTIN,OUTPUT);

Serial.begin(115200);
delay(600);
Serial.println("");
Serial.println("test presence polling");
delay (2000);

}

void loop() {
int p=digitalRead(IN);

digitalWrite(LED_BUILTIN,!p); //LED is active LOW
Serial.println(p); 
delay(100);


}
