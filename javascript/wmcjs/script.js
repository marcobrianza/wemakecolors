var mqtt;
var reconnectTimeout = 2000;

host = 'net.marcobrianza.it';	// hostname or IP address
port = 9001;
topic = '#';		// topic to subscribe to
useTLS = false;
username = null;
password = null;

cleansession = true;

function MQTTconnect() {

  if (typeof path == "undefined") {
    path = '/WeMakeColors/color';
  }

  mqtt = new Paho.MQTT.Client(
    host,
    port,
    path,
    "web_" + parseInt(Math.random() * 100, 10)
  );

  var options = {
      timeout: 3,
      useSSL: useTLS,
      cleanSession: cleansession,
      onSuccess: onConnect,
      onFailure: function (message) {
          $('#status').val("Connection failed: " + message.errorMessage + "Retrying");
          setTimeout(MQTTconnect, reconnectTimeout);
      }
  };

  mqtt.onConnectionLost = onConnectionLost;
  mqtt.onMessageArrived = onMessageArrived;

  if (username != null) {
      options.userName = username;
      options.password = password;
  }
  console.log("Host="+ host + ", port=" + port + ", path=" + path + " TLS = " + useTLS + " username=" + username + " password=" + password);
  mqtt.connect(options);
}

function onConnect() {
    $('#status').val('Connected to ' + host + ':' + port + path);
    mqtt.subscribe(topic, {qos: 0});
    $('#topic').val(topic);
}

function onConnectionLost(response) {
    setTimeout(MQTTconnect, reconnectTimeout);
    $('#status').val("connection lost: " + responseObject.errorMessage + ". Reconnecting");
};

function onMessageArrived(message) {

    var topic = message.destinationName;
    var payload = message.payloadString;

    console.log('raw r', message.payloadBytes[0] )
    console.log('raw g', message.payloadBytes[1] )
    console.log('raw b', message.payloadBytes[2] )

    var colors = message.payloadBytes.map( (x) => x*16)
    console.log(colors)

    var c = `rgb(${colors[0]},${colors[1]},${colors[2]})`;
    $('#ws').prepend(`<li class="color-block" style="background-color:${c}"></li>`);
};


$(document).click( function() {
  console.log('cludee click')

  var uint8 = new Uint8Array(3);
  uint8.set([15,2,3]);

  console.log(uint8)
  mqtt.send('/WeMakeColors/color', uint8)
})

$(document).ready(function() {
    MQTTconnect();
});
