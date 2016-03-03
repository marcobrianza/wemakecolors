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
color CC=color(127,2,3); // colors must be < 128 becauuse I think there is an encoding problem somewhere
//color CC=color(200,2,3); // this causes issues

MQTTClient client;

String clientID= "processingColor";
//String mqttURL="mqtt://try:try@broker.shiftr.io";
String mqttURL="mqtt://test.mosquitto.org";

void setup() {


  c=new byte[MSG_SIZE];
    
  
  c[0]=byte(red(CC));
  c[1]=byte(green(CC));
  c[2]=byte(blue(CC));


  client = new MQTTClient(this);
  client.connect(mqttURL, clientID);
  client.subscribe("/color");
  // client.unsubscribe("/color");
  
}

void draw() {
}

void mousePressed() {
  client.publish("/color", c);
}

void messageReceived(String topic, byte[] payload) {
  print("new message: " + topic + " - "+ payload.length+" - ");
  if ( payload.length==3) {
    println(payload[0], payload[1], payload[2]);
    newC=color(payload[0], payload[1], payload[2]);
    background(newC);
  }
}