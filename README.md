“We Make Colors” is a network of artworks that dynamically generate random colors based on a physical presence near each device.
All copies of the artwork generate and broadcast to each other the new colors in real time using MQTT, current standard protocol for IoT.
The artworks create an unintentional interaction between the spaces and persons. The colors scroll in the device like in a social network timeline: new colors enter from the top and old ones exit from the bottom. All the devices have the same colors at the same time.

The colors of each device are produced by a strip of 9 RGB addressable LEDs.
The new color is generated when the passive infrared sensor interrupts the microprocessor count loop, typically when a human is passing by. There is no identification of the device that generates the new color.
To avoid information flooding and an intentional interaction, the maximum color frequency is one per minute per each device.
The processor is based on the Wi-Fi SoC ESP8266 programmed with the Arduino IDE. The device is open source and can be made with a 60×30 cm laser cutter, available in every Fablab. The main construction element is a 3 mm acrylic sheet. The bill of the material is below 20 €, including a 3 $ donation to Arduino.cc for the IDE support.