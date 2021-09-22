# Mqtt button
This is just another relay control via MQTT using ESP8266 (Wemos D1).

## Build
To build this [Platform io](https://platformio.org) project just pull, rename [config.h_sample.h](include/config.h_sample.h) to `config.h`, modify [platformio.ini](platformio.ini) with your board and COM details, compile, upload.

## Features
* connect to wifi
* connect to mqtt server
* subscribe to command topic
* listen for json command
* send status update (mqtt message) on relay on and relay off
* send heartbeat messages to mqtt
* supports OTA - see env entry in `platformio.ini`

## Command
So far only following json body is supported

    {"push": 2000}

By this example it "pushes the button" for 2000ms. It means it brings relay pin HIGH for given number of milliseconds then put it low again. 
I might add ON/OFF commands later but for now this is enough.
