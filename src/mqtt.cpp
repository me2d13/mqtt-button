#include <ESP8266WiFi.h>
#include "mqtt.h"
#include <WifiClient.h>
#include "config.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient net;
PubSubClient client(net);
StaticJsonDocument<200> doc;
pushHandlerType pushHandler = NULL;

char buffer[100];

void connectMqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ring")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      sprintf(buffer, "LR relay connected to MQTT with IP  %s", WiFi.localIP().toString().c_str());
      mqttLog(buffer);
      // ... and resubscribe
      client.subscribe(MQTT_SUB_TOPIC);
    } else {
      Serial.print("Mqtt connect failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttHeartBeat() {
    mqttLog("LR relay alive");
}

void logState(int value) {
  if (client.connected()) {
    sprintf(buffer, "LR relay state  %d", value);
    client.publish(MQTT_PUB_TOPIC,buffer);
  }
}

void mqttLog(char *message) {
  if (client.connected()) {
    client.publish(MQTT_LOG_TOPIC,message);
  }
}

void setPushHandler(pushHandlerType handler) {
    pushHandler = handler;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  long time = doc["push"];
  if (time > 0) {
      Serial.print("Pressing button for ");
      Serial.print(time);
      Serial.println(" miliseconds");
      if (pushHandler != NULL) {
          pushHandler(time);
      } else {
          Serial.println("WARNING: push handler not set");
      }
  }
}

void setupMqtt() {
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void mqttLoop() {
  if (!client.connected()) {
    connectMqtt();
  }
  client.loop();
}