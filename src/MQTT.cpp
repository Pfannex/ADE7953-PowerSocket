#include "MQTT.h"

//###############################################################################
//  MQTT client
//###############################################################################
MQTT::MQTT(SysUtils& sysUtils, API& api, FFS& ffs, I2C& i2c, WIFI& wifi):
    sysUtils(sysUtils),
    api(api),
    ffs(ffs),
    i2c(i2c),
    wifi(wifi),
    client(wifi.client){

//callbacks
    client.setCallback(std::bind(&MQTT::on_incommingSubcribe, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

//-------------------------------------------------------------------------------
//  MQTT public
//-------------------------------------------------------------------------------
//...............................................................................
//  WiFi start connection
//...............................................................................
bool MQTT::start(){
  bool MQTTOK = false;

  String strIP = ffs.cfg.readItem("mqttServer");
  IPAddress IP = sysUtils.net.strToIP(strIP);
  int port = ffs.cfg.readItem("mqttPort").toInt();
  String deviceName = ffs.cfg.readItem("mqttDeviceName");
  String lastWillTopic = "Devices/" + deviceName;

  sysUtils.logging.log("MQTT", "connecting to: " + strIP + ":" + String(port));
  i2c.lcd.clear();
  i2c.lcd.println("MQTTBroker:", ArialMT_Plain_10, 0);
  i2c.lcd.println(strIP + ":" + String(port), ArialMT_Plain_16,  10);

  client.setServer(IP, port);
  client.disconnect();
  if (client.connect(deviceName.c_str(), lastWillTopic.c_str() , 0, false, "Dead")) {
    MQTTOK = true;
    sysUtils.logging.log("MQTT", "connected to Broker");
    i2c.lcd.println("...connected", ArialMT_Plain_10, 31);
    client.publish(lastWillTopic.c_str(), "Alive");

    //global subscribe
    DynamicJsonBuffer global_JsonBuffer;
    JsonObject& global_rootObject = global_JsonBuffer.parseObject(ffs.subGlobal.root);
    for (auto &element : global_rootObject){
      //String strKey = element.key;
      String strValue = element.value; strValue += "/#";
      //Serial.println(strValue);
      client.subscribe(strValue.c_str());
      client.loop();
    }
    //device subscribe
    DynamicJsonBuffer device_JsonBuffer;
    JsonObject& device_rootObject = device_JsonBuffer.parseObject(ffs.sub.root);
    for (auto &element : device_rootObject){
      //String strKey = element.key;
      String value = element.value;
      String strValue = deviceName; strValue += "/"; strValue += value;
      //Serial.println(strValue);
      client.subscribe(strValue.c_str());
      client.loop();
    }
  }
  return MQTTOK;
}

//...............................................................................
//  WiFi start connection
//...............................................................................
bool MQTT::handle(){
  client.loop();
  if (client.connected())
    return true;
  else
    return false;
}

//...............................................................................
//  EVENT incomming MQTT subcribe
//...............................................................................
void MQTT::on_incommingSubcribe(char* topic, byte* payload, unsigned int length){
//topic
  String strTopic = String(topic);
  String deviceName = ffs.cfg.readItem("mqttDeviceName");
  if (strTopic.indexOf(deviceName) != -1) {  //is Topic with DeviceName
    strTopic.remove(0, deviceName.length() +1);
  }
//argument
  char arg[500] = "";
  for (int i = 0; i < length; i++) {
    arg[i] = payload[i];
  }
  String strArg = String(arg);
  TTopic tmpTopic = api.dissectTopic(strTopic, strArg);

  api.call(tmpTopic);
}

//-------------------------------------------------------------------------------
//  MQTT private
//-------------------------------------------------------------------------------
