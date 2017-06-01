#include "MQTT.h"

//###############################################################################
//  MQTT client 
//###############################################################################
MQTT::MQTT(FFS& ffs, I2C& i2c, WIFI& wifi):
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
  IPAddress IP = h.strToIP(strIP);
  int port = ffs.cfg.readItem("mqttPort").toInt();
  String deviceName = ffs.cfg.readItem("mqttDeviceName");
  String lastWillTopic = "Devices/" + deviceName;

  Serial.print("Connecting to MQTT-Broker: ");  
  Serial.println(strIP);
  i2c.lcd.clear();
  i2c.lcd.println("Connecting to MQTTBroker:", ArialMT_Plain_10, 0);  
  i2c.lcd.println(strIP + ":" + String(port), ArialMT_Plain_16,  10);  
  
  client.setServer(IP, port); 
  client.disconnect();
  //if (client.connect(ffs.cfg.readItem("mqttDeviceName").c_str())){
  if (client.connect(deviceName.c_str(), lastWillTopic.c_str() , 0, false, "Dead")) {
    MQTTOK = true;
    Serial.println("MQTT connected");
    i2c.lcd.println("connected to MQTTBroker", ArialMT_Plain_10, 31);  
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
  String strTopic = String(topic);
  char value[500] = "";
  String attr[10];
  int attrCount = 0;  
  
  for (int i = 0; i < length; i++) {
    value[i] = payload[i];
    char x = payload[i];
    String y = String(x);
    if (y == "," ){
      attrCount++;     
    }else{
      attr[attrCount] += (char)payload[i];
    }
  }
  attrCount++;
  String strValue = String(value);
  
  Serial.println("MQTT::on_incommingSubcribe");
  Serial.println(strTopic + " | " + strValue);
  for (int i=0; i<attrCount; i++){
    Serial.println("   " + String(i+1) + ": " + attr[i]);   
  }
}

//-------------------------------------------------------------------------------
//  MQTT private
//-------------------------------------------------------------------------------

