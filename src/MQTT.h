#pragma once
#include "API.h"
#include "Setup.h"
#include <Arduino.h>
#include <IPAddress.h>
#include <PubSubClient.h>

#include <functional>
typedef std::function<void(void)> CallbackFunction;
typedef std::function<void(Topic&)> Topic_CallbackFunction;

//###############################################################################
//  MQTT client
//###############################################################################
class MQTT {
public:
  MQTT(API &api);
  API &api;

  void setCallback(MQTT_CALLBACK_SIGNATURE);

  bool start();
  bool handle();
  // Callback Events
  // pubSubClient
  void on_incommingSubcribe(char *topic, byte *payload, unsigned int length);
  // API
  void on_pubMQTT(Topic &topic);

  void pub(String topic, String value);

private:
  WiFiClient espClient;
  PubSubClient client;
};
