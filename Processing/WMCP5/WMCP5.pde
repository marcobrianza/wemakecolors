// We Make Colors Processing edition
// Marco Brianza, April 2015 

String clientID= "processingColor";  // *** Change the Client ID for each client ***//

String mqttURL="mqtt://wmc.marcobrianza.it:1883";
import mqtt.*; // MQTT library by Joel Gaehwiler (use add library from the menu)
MQTTClient client;


int MAX_C=16;
int MSG_SIZE=3;
byte c[]=new byte[MSG_SIZE];
color newC;
color[] colors = new color[9];


void setup() {
  size(100, 900);
  client = new MQTTClient(this);
  client.connect(mqttURL, clientID);
  client.subscribe("/WeMakeColors/color");
  noStroke();
}

void draw() {
}

void mousePressed() {
  byte r=byte(random(0, MAX_C));
  byte g=byte(random(0, MAX_C));
  byte b=byte(random(0, MAX_C)); 

  c[0]=r;
  c[1]=g;
  c[2]=b;

  println("My random color:", r, g, b);
  client.publish("/WeMakeColors/color", c);
}


void connectionLost() {
  println("connection lost");
}

void messageReceived(String topic, byte[] payload) {
  print("New message:" + topic + " - ("+ payload.length+") - ");
  int r, g, b;
  if (payload.length==MSG_SIZE) {
    r=payload[0]& 0xff;
    g=payload[1]& 0xff;
    b=payload[2]& 0xff;

    println(r, g, b);
    int R=cal_lut(r);
    int G=cal_lut(g);
    int B=cal_lut(b);

    newC=color(R, G, B);
    newColor(newC);
  }
}

void newColor(color c) {
  int i;
  for (i=8; i>0; i--) {
    colors[i]=colors[i-1];
  }
  colors[0]=c;

  for (i=0; i<9; i++) {
    fill(colors[i]);
    rect(0, i*100, 100, 100);
  }
}


int cal_lut(int c) {
  if (c>15) c=15;
  int[] lut = {0, 11, 14, 17, 21, 27, 34, 42, 53, 66, 83, 104, 130, 163, 204, 255};
  return lut[c];
}