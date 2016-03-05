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
color CC=color(255, 2, 3); //test color=red

MQTTClient client;

String clientID= "processingColor";
//String mqttURL="mqtt://try:try@broker.shiftr.io";
//String mqttURL="mqtt://test.mosquitto.org";
String mqttURL="mqtt://net.marcobrianza.it";


void setup() {

  c=new byte[MSG_SIZE]; 

  c[0]=byte(red(CC));
  c[1]=byte(green(CC));
  c[2]=byte(blue(CC));

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
  newC=color(r, g, b);
  background(newC);
  }
}