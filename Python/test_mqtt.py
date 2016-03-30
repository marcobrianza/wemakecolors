# https://pypi.python.org/pypi/paho-mqtt/1.1
import paho.mqtt.client as mqtt


mqtt_server="net.marcobrianza.it"

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe("$SYS/#")
    client.subscribe("/WeMakeColors/#")
    
def on_disconnect(client, userdata, rc):
    print "disconect"


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic)
    #print(userdata)
    print "len" ,len(msg.payload)
    print "data" ,ord(msg.payload[0]),ord(msg.payload[1]),ord(msg.payload[2])

    #client.publish("/test",msg.payload)
    
    
    
client = mqtt.Client("py_client",True,"my_user_data")
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect


client.connect(mqtt_server, 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()