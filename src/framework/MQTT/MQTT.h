#pragma once
#include "framework/OmniESP/API.h"
// setup before pubsubclient to have MQTT_MAX_PACKET_SIZE  already defined
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

  void setCallback(MQTT_CALLBACK_SIGNATURE);

  bool start();
  bool stop();
  bool handle();
  // Callback Events
  // pubSubClient
  void on_incomingSubscribe(char *topic, byte *payload, unsigned int length);
  // API
  void on_logFunction(const String &channel, const String &msg);
  void on_topicFunction(const time_t, Topic &topic);

private:
  void tryReconnect();
  bool state = 0;
  bool wifiState = 0;
  String deviceName;
  WiFiClient espClient;
  PubSubClient client;
  void pub(const char* topic, const char* value);
  void pub(String& topic, const String& value);
};
