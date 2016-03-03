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
color CC=color(255,2,3); 

MQTTClient client;

String clientID= "processingColor";
//String mqttURL="mqtt://try:try@broker.shiftr.io";
//String mqttURL="mqtt://test.mosquitto.org";
String mqttURL="mqtt://broker.hivemq.com";


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
    int r,g,b;
    r=payload[0]& 0xff;
    g=payload[1]& 0xff;
    b=payload[2]& 0xff;
    
    println(r,g,b);
    newC=color(r,g,b);
    background(newC);
  }
}