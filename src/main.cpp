#include <Arduino.h>
#include <Ticker.h>
#include "config.h"
#include "ota.h"
#include "mqtt.h"

// mosquitto_sub -t '/devices/relay/door/log'

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void blick();
void release();

Ticker heartBeatTimer(mqttHeartBeat, HEART_BEAT_S*1000);
Ticker blickTimer(blick, BLICK_S*1000);
Ticker* releaseTimerPtr;

void setupAndWaitForWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  WiFi.setAutoReconnect(true);
}

void press() {
  digitalWrite(PIN_R, HIGH);
  logState(1);
}

void release() {
  digitalWrite(PIN_R, LOW);
  logState(0);
  delete releaseTimerPtr;
  releaseTimerPtr = NULL;
}

void blick() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(5);
  digitalWrite(LED_BUILTIN, HIGH);
}

void pushHandler(long time) {
  press();
  releaseTimerPtr = new Ticker(release, time, 1, MILLIS);
  releaseTimerPtr->start();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // initialize onboard LED as output
  pinMode(PIN_R, OUTPUT);  // initialize onboard LED as output
  Serial.begin(115200);
  Serial.println("Booting");
  setupAndWaitForWifi();
  setupOTA();
  setupMqtt();
  connectMqtt();
  setPushHandler(&pushHandler);
  heartBeatTimer.start();
  blickTimer.start();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  ArduinoOTA.handle();
  mqttLoop();
  heartBeatTimer.update();
  blickTimer.update();
  if (releaseTimerPtr != NULL) {
    releaseTimerPtr->update();
  }
}