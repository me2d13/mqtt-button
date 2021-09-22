#ifndef MQTT_H
#define MQTT_H

typedef void (*pushHandlerType)(long);


void mqttHeartBeat();
void setupMqtt();
void connectMqtt();
void mqttLoop();
void mqttLog(char *message);
void setPushHandler(pushHandlerType handler);
void logState(int value);

#endif