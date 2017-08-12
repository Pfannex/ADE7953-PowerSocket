#pragma once
#include "API.h"
#include "Setup.h"
#include <Arduino.h>
#include <IPAddress.h>
#include <PubSubClient.h>

//###############################################################################
//  MQTT client
//###############################################################################
class MQTT {
public:
  MQTT(API &api);
  API &api;

  void on_pubMQTT();

  bool start();
  bool handle();
  void setCallback(MQTT_CALLBACK_SIGNATURE);
  void on_incommingSubcribe(char *topic, byte *payload, unsigned int length);
  void pub(String topic, String value);

private:
  WiFiClient espClient;
  PubSubClient client;
};
