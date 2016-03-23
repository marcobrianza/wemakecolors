

//#define IN 7

#define IN_PIN D2
#define MIN_TIME 60000 //delay between new color

volatile boolean newPresence = false;

//random color stuff
byte COLOR_BITS = 5;
unsigned long MAX_C = pow(2,COLOR_BITS); // number of variaitons per each color
byte COLOR_MASK = MAX_C - 1;
unsigned long MAX_NUM =  pow(MAX_C, 3);

unsigned long rnd;
unsigned long loops;
byte r, g, b;

void setup() {
  // put your setup code here, to run once:
  pinMode(IN_PIN, INPUT);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("We Make Colors");
  attachInterrupt(digitalPinToInterrupt(IN_PIN), presence_isr, RISING);

  Serial.println(COLOR_BITS);
  Serial.println(MAX_C);
  Serial.println(COLOR_MASK);
  Serial.println(MAX_NUM);
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

  r = c      & COLOR_MASK; // red
  g = c >> 4 & COLOR_MASK; //green
  b = c >> 8 & COLOR_MASK; //blue

  Serial.print ("spins ");
  Serial.println (loops / MAX_NUM);

  Serial.println("rgb ");
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  Serial.println("");

}


