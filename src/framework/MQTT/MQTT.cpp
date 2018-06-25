#include "framework/MQTT/MQTT.h"
#include "framework/Utils/SysUtils.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

//###############################################################################
//  MQTT client
//###############################################################################
MQTT::MQTT(API &api) : api(api), espClient(), client(espClient) {

  // callbacks
  // pubSubClient
  client.setCallback(std::bind(&MQTT::on_incomingSubscribe, this,
                               std::placeholders::_1, std::placeholders::_2,
                               std::placeholders::_3));
  // API
  // register log function
  api.registerLogFunction(std::bind(&MQTT::on_logFunction, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2));

  // register Topic function
  api.registerTopicFunction(std::bind(&MQTT::on_topicFunction, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
}

//-------------------------------------------------------------------------------
//  MQTT public
//-------------------------------------------------------------------------------

//...............................................................................
//  MQTT start connection
//...............................................................................
bool MQTT::start() {
  bool MQTTOK = false;

  if (api.call("~/get/ffs/cfg/item/mqtt") == "on") {
    String strIP = api.call("~/get/ffs/cfg/item/mqtt_ip");
    IPAddress IP = SysUtils::strToIP(strIP);
    int port = api.call("~/get/ffs/cfg/item/mqtt_port").toInt();
    deviceName = api.call("~/get/ffs/cfg/item/device_name");
    String lastWillTopic = "Devices/" + deviceName;

    api.info("MQTT client connecting to " + strIP + ":" + String(port));
    api.info("MQTT DeviceName: " + deviceName);

    client.disconnect();
    client.setServer(IP, port);
    if (client.connect(deviceName.c_str(), lastWillTopic.c_str(), 0, false,
                       "Dead")) {
      MQTTOK = true;
      api.info("MQTT client connected to MQTT broker");

      client.publish(lastWillTopic.c_str(), "Alive");

      String root;

      // global subscribe
      String aliases = api.call("~/get/ffs/cfg/item/mqtt_aliases");

      char *alias;
      char *s = strdup(aliases.c_str());
      alias = strtok(s, ",; ");
      while (alias != NULL) {
        if (*alias) {

          String strValue = String(alias);  //ggf. strdup wg. free(s)
          strValue += "/#";                 //ggf. NUR set/get!?
          //Serial.println(strValue);
          client.subscribe(strValue.c_str());
          client.loop();
        }
        alias = strtok(NULL, ",; ");
      }
      free(s);

      // device subscribe
      String subStr = deviceName + "/set/#";
      client.subscribe(subStr.c_str());
      client.loop();
      subStr = deviceName + "/get/#";
      client.subscribe(subStr.c_str());
      client.loop();

    }
  } else {
    api.info("MQTT is switched off");
  }

  return MQTTOK;
}

//...............................................................................
//  MQTT stop connection
//...............................................................................
bool MQTT::stop() {
  bool MQTTOK = false;
  client.disconnect();
  api.info("MQTT client has disconnected");
  return MQTTOK;
}

//...............................................................................
//  WiFi start connection
//...............................................................................
bool MQTT::handle() {
  if (client.connected()) {
    client.loop();
    return true;
  } else
    return false;
}

//...............................................................................
//  EVENT incomming MQTT subcribe
//...............................................................................
void MQTT::on_incomingSubscribe(char *topics, byte *payload,
                                unsigned int length) {

  // If we do
  //   String str = String(topics) + " " + String(args);
  // and pass str through
  //   api.debug(str)
  // then topics but not args is destroyed. We should investigate the reason!

  // workaround
  char *topics2 = strdup(topics);
  char *args = new char[length + 1];
  strncpy(args, (char *)payload, length);
  args[length] = '\0';

  // This is the actual action. We throw away the result because the API
  // itself cares about informing the listeners/views about the executed
  // command and its result, see API::call().
  api.call(String(topics2) + " " + String(args));

  free(topics2);
  if (args != NULL)
    delete[] args;
}

//...............................................................................
//  MQTT publish API log
//...............................................................................
void MQTT::on_logFunction(const String &channel, const String &msg) {

  // do not use API calls here to avoid infinite recursions

  String item;
  if(channel == " INFO") item="info"; else
  if(channel == "DEBUG") item="debug"; else
  if(channel == "ERROR") item="error";
  String logTopic = deviceName + "/log/" + item;
  pub(logTopic, msg);
}

//...............................................................................
//  MQTT publish API Topic
//...............................................................................
void MQTT::on_topicFunction(const time_t, Topic &topic) {

  // be careful with API calls here to avoid infinite recursions

  String tail= topic.modifyTopic(0);
  // First react on events that affect us...
  String topicStr = "~/" + tail;

  if (topicStr == "~/event/net/connected") {
    if (topic.getArgAsLong(0)) {
      start(); // start MQTT
    } else {
      stop();  // stop MQTT
    }
  }

  // ..then publish the topic.
  pub(topic.topic_asString(), topic.arg_asString());
}

//-------------------------------------------------------------------------------
//  MQTT private
//-------------------------------------------------------------------------------

//...............................................................................
//  MQTT publish
//...............................................................................
void MQTT::pub(String topic, String value) {
  if (client.connected()) {
    client.publish(topic.c_str(), value.c_str());
    client.loop();
  }
}
