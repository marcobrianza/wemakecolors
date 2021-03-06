// test software for We Make Colors
// Marco Brianza - April 9th 2016
//
// test presence and rnd


#if defined(ESP8266)
#define IN_PIN D2
#endif

#if defined(ARDUINO_SAMD_MKR1000)
#define IN_PIN 4
#endif

#define MIN_TIME 6000 //delay between new color

volatile boolean newPresence = false;

//random color stuff
unsigned long MAX_C = 16; // number of variaitons per each color
unsigned long MAX_CCC = MAX_C * MAX_C * MAX_C;
unsigned long MAX_NUM = MAX_CCC;

unsigned long rnd;
unsigned long loops;
byte r, g, b;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n test presence and rnd");

  pinMode(IN_PIN, INPUT);

  int i;

#if defined(ESP8266)
  i = digitalPinToInterrupt(IN_PIN);
#endif

#if defined(ARDUINO_ARCH_SAMD)
  i = IN_PIN;
#endif

  attachInterrupt(i, presence_isr, RISING);

}

void loop() {
  loops++;
  if (newPresence) {
    newPresence = false;
    Serial.print("sec ");
    Serial.println(millis() / 1000);
    rnd_color();
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


void rnd_color() {
  unsigned long c = loops % MAX_NUM;

  r = c    % MAX_C         ; // red
  g = c / MAX_C  % MAX_C; //green
  b = c / MAX_C  / MAX_C % MAX_C; //blue

  Serial.print ("spins ");
  Serial.println (loops / MAX_NUM);

  Serial.println("rgb ");
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  Serial.println("");

}


