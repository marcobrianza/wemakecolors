// This example sketch connects to shiftr.io
// and sends a message on every mouse clic.
//
// After starting the sketch you can find the
// client here: https://shiftr.io/try.
//
// originaly by Joël Gähwiler
// https://github.com/256dpi/processing-mqtt

import mqtt.*;

int MSG_SIZE=3;
byte c[];

color newC;

MQTTClient client;

String clientID= "processingColor";
String mqttURL="mqtt://net.marcobrianza.it";
//String mqttURL="mqtt://try:try@broker.shiftr.io";
//String mqttURL="mqtt://test.mosquitto.org";
//String mqttURL="mqtt://192.168.1.129";

void setup() {

  c=new byte[MSG_SIZE]; 

  c[0]=15;
  c[1]=2;
  c[2]=3;

  client = new MQTTClient(this);
  client.connect(mqttURL, clientID);
  client.subscribe("/WeMakeColors/color");
  // client.unsubscribe("/color");
}

void draw() {
}

void mousePressed() {
  client.publish("/WeMakeColors/color", c);
}

void keyPressed() {
  if (key == 'r') {
  c[0]=15;
  c[1]=0;
  c[2]=0;
  }
    if (key == 'g') {
  c[0]=0;
  c[1]=15;
  c[2]=0;
  }
    if (key == 'b') {
  c[0]=0;
  c[1]=0;
  c[2]=15;
  }
   if (key == ' ') {
  c[0]=0;
  c[1]=0;
  c[2]=0;
  } 
  
   client.publish("/WeMakeColors/color", c); 
}


void connectionLost() {
  println("### connection lost ##");
}

void messageReceived(String topic, byte[] payload) {
  print("new message: " + topic + " - "+ payload.length+" - ");
  int r, g, b;
  if (payload.length==MSG_SIZE){
  r=payload[0]& 0xff;
  g=payload[1]& 0xff;
  b=payload[2]& 0xff;

  println(r, g, b);
  newC=color(calibrate(r,g,b));
  background(newC);
  }
}

color calibrate(int R, int G, int B) {
  float gamma=2.5;
  float outRange=255;
  float inRange=15;
  
  float rC, gC, bC;
  color cC;

  float r=float(R);
  float g=float(G);
  float b=float(B);

  rC=pow((r/inRange), gamma)*outRange;
  gC=pow((g/inRange), gamma)*outRange;
  bC=pow((b/inRange), gamma)*outRange;

  cC=color(rC, gC, bC); 

  return(cC);
}