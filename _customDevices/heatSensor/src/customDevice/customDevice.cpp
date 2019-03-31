#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs),
      ow("oneWire", logging, topicQueue, OWPIN, ffs)
      {

  //callback Events
  ow.set_callbacks(std::bind(&customDevice::on_SensorChanged, this, std::placeholders::_1),
                   std::bind(&customDevice::on_SensorData, this, std::placeholders::_1));

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// sensors callback
//...............................................................................
void customDevice::on_SensorChanged(String json) {
  DynamicJsonBuffer root;
  JsonObject& sensors = root.parseObject(json);
  Serial.println("----------------------------------------");
  Serial.println("customDevice::on_SensorChanged");
  sensors.prettyPrintTo(Serial);
  Serial.println("");
  Serial.println("----------------------------------------");

  modifySensorConfiguration(json);
}
void customDevice::on_SensorData(String json) {
  DynamicJsonBuffer root;
  JsonObject& sensors = root.parseObject(json);
  Serial.println("----------------------------------------");
  Serial.println("customDevice::on_SensorData");
  sensors.prettyPrintTo(Serial);
  Serial.println("");
  Serial.println("----------------------------------------");

  updateDashboard(json);
}

//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  Device::start();
  sensorPollTime = ffs.deviceCFG.readItem("cfg_sensorPollTime").toInt();
  logging.info("1wire sensors polling time: " + String(sensorPollTime));
  logging.info("starting device " + String(DEVICETYPE) + String(DEVICEVERSION));

  ow.start();
  ow.owPoll = sensorPollTime;

  logging.info("device running");

  logging.info("scanning I2C-Bus for devices");
  logging.info(Wire.i2c.scanBus());

}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {
  ow.handle();
  unsigned long now = millis();
  if (now - lastPoll >= sensorPollTime) {
    lastPoll = now;
    //readBMP180("sensor1");
    //readSi7021("Si7021");
  }
}

//...............................................................................
//  Device set
//...............................................................................

String customDevice::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─deviceCFG        (level 3)
    └─index            (level 4) individual setting
    └─value            (level 4) individual setting
    └─scanBus          (level 4) scan 1W-Bus for new devices
*/


  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());
  String ret = TOPIC_NO;

  if (topic.itemIs(3, "deviceCFG")) {
    //index-------------------
    if (topic.itemIs(4, "index")) {
      int count = ffs.deviceCFG.itemsCount;
      String strIndex = topic.getArg(0);
      if (strIndex == "next"){
        (index < count) ? (index++) : (index = 1);
        pub_deviceCFGItem();
        return TOPIC_OK;
      }else if (strIndex == "previous"){
        (index > 1) ? (index--) : (index = count);
        pub_deviceCFGItem();
        return TOPIC_OK;
      }
    }
    //set key value-------------------
    if (topic.itemIs(4, "value")) {
      set_deviceCFGItem(topic.getArg(0));
      return TOPIC_OK;
    }
    //scan 1W-bus-------------------
    if (topic.itemIs(4, "scanBus")) {
      ow.scanBus();
      return TOPIC_OK;
    }

  }

  return ret;
}

//...............................................................................
//  Device get
//...............................................................................

String customDevice::get(Topic &topic) {
  /*
  ~/get
  └─device             (level 2)
    └─sensor1          (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/sensor1
    return TOPIC_NO;
  if (topic.itemIs(3, "sensor1")) {
    return "";
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// event handler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {
/*
  if (topic.modifyTopic(0) == "event/device/sensorsChanged") {
    on_sensorsChangedx(topic.arg_asString());
  }
  if (topic.modifyTopic(0) == "event/device/sensorsData") {
    on_pushToDashboard(topic.getArg(0));
  }*/

}

//...............................................................................
//  on request, fillDashboard with values
//...............................................................................
String customDevice::fillDashboard() {

  pub_deviceCFGItem();
  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

//...............................................................................
// publish key:value to dashboard by index
//...............................................................................
void customDevice::pub_deviceCFGItem(){
  //deviceCFG json
  DynamicJsonBuffer ffs_root;
  JsonObject& ffs_deviceCFG = ffs_root.parseObject(ffs.deviceCFG.root);

  int i = 1;
  String key = "";
  String value = "";
  for (auto kv : ffs_deviceCFG) {
    key = String(kv.key);
    value = String(kv.value.as<char*>());
    if (i == index) break;
    i++;
  }
  topicQueue.put("~/event/device/deviceCFG/key " + key);
  topicQueue.put("~/event/device/deviceCFG/value " + value);
}

//...............................................................................
// store value to ffs
//...............................................................................
void customDevice::set_deviceCFGItem(String value){
  //deviceCFG json
  DynamicJsonBuffer ffs_root;
  JsonObject& ffs_deviceCFG = ffs_root.parseObject(ffs.deviceCFG.root);

  int i = 1;
  String key = "";
  //String value = "";
  for (auto kv : ffs_deviceCFG) {
    key = String(kv.key);
    //value = String(kv.value.as<char*>());
    if (i == index) break;
    i++;
  }
  //change polltime
  if (key == "cfg_sensorPollTime"){
    sensorPollTime = value.toInt();
    ow.owPoll = sensorPollTime;
  }

  //if (!ffs_deviceCFG.containsKey(key)){
    ffs.deviceCFG.writeItem(key, value);
    ffs.deviceCFG.saveFile();
  //}
  //ffs_deviceCFG.prettyPrintTo(Serial);

  //modify Dashboard
  //Serial.println(ffs.deviceCFG.root);

  //modifyDashboard();
  //dashboardChanged();

}























//...............................................................................
// update dashboard with measured data
//...............................................................................
void customDevice::updateDashboard(String json){
  DynamicJsonBuffer root;
  JsonObject& sensors = root.parseObject(json);
  DynamicJsonBuffer ffs_root;
  JsonObject& ffs_sensors = root.parseObject(ffs.deviceCFG.root);

  String key = "";
  String value = "";
  String alias = "";
  for (auto kv : sensors) {
    key = String(kv.key);
    value = String(kv.value.as<char*>());

    for (auto kv : ffs_sensors) {
      if (key == String(kv.key)) {
        key = String(kv.key);
        break;
      }
    }

    topicQueue.put("~/event/device/sensors/" + key + " " + value);

    //fill Dashboard
    //Serial.println("updateDashboard");
    //Serial.println(key + ":" + value);
  }
}

//...............................................................................
//modify ffs & dashboard
//...............................................................................
void customDevice::modifySensorConfiguration(String json){
  //logging.info("SensorsChanged");
  //Serial.println(json);
  //owSensors = json;

  DynamicJsonBuffer root;
  JsonObject& sensors = root.parseObject(json);
  DynamicJsonBuffer ffs_root;
  JsonObject& ffs_sensors = ffs_root.parseObject(ffs.deviceCFG.root);

  //delete removed sensors (not the configKeys!)
  for (auto kv : ffs_sensors) {
    String key = String(kv.key);
    if (!sensors.containsKey(key) and !key.startsWith("cfg_")){  //key in deviceCFG does not exist
      ffs_sensors.remove(key);
    }
  }

  //add new keys to ffs & syncronize dynamicDashboard
  //int id = 0;
  for (auto kv : sensors) {
    String key = String(kv.key);
    String value = String(kv.value.as<char*>());

    //add new Sensor to ffs
    if (!ffs_sensors.containsKey(key)){
      ffs_sensors[key] = "NEW_" + key;    //add new key
    }

    //String ffsKey = ffs_sensors[key];
    //Serial.println("NEW? " + ffsKey);



  }
  ffs.deviceCFG.root = "";
  ffs_sensors.printTo(ffs.deviceCFG.root);
  //Serial.println("--------------------------------------");
  //ffs_sensors.prettyPrintTo(Serial);
  //Serial.println("");
  //Serial.println("--------------------------------------");
  ffs.deviceCFG.saveFile();
  modifyDashboard();

}

//...............................................................................
// mofify dynamicDashboard
//...............................................................................
void customDevice::modifyDashboard(){
  logging.info("modify Dashboard");

  DynamicJsonBuffer ffs_root;
  JsonObject& ffs_sensors = ffs_root.parseObject(ffs.deviceCFG.root);

  //if (!containsKey(groupName)) dashboard.removeWidget(groupName);
  String groupName = "sensors";
  dashboard.removeWidget(groupName);
  Widget* w = dashboard.insertWidget("group", 1);
    w->name= groupName;
    w->caption = "Sensorenmesswerte Heizung";
    //grid= (WidgetGrid*) dashboard.insertWidget("grid", "group_sensors");

  for (auto kv : ffs_sensors) {
    String key = String(kv.key);
    String value = String(kv.value.as<char*>());
    if (!key.startsWith("cfg_")){

      Widget* w1 = dashboard.insertWidget("text", "sensors");
      w1->name = key;
      w1->caption = value;
      w1->value = "";
      w1->event = "~/event/device/sensors/" + key;

    }
  }

  //DynamicJsonBuffer dashboard_root;
  //JsonArray& dashboard_array = dashboard.serialize(dashboard_root);
  //JsonArray& sensor_data = dashboard_array[1]["data"];

  //Serial.println("--------------------------------------");
  //dashboard_array.prettyPrintTo(Serial);
  //Serial.println("");
  //Serial.println("--------------------------------------");

  Serial.println("dashboardChanged()");
  //topicQueue.put("~/event/device/dashboardChanged");
  dashboardChanged();


  //dashboard.removeWidget(key);
  // if widget name not exists!!

/*
  if (!containsKey(key)){
    Widget* w = dashboard.insertWidget("text", "sensors");
    w->name = key;
    w->caption = key;
    w->value = value;
    w->event = "~/event/device/sensors/" + key;
  }
*/


}

//...............................................................................
// search for key in dynamicDashboard
//...............................................................................
bool customDevice::containsKey(String key){
  //syncronize dynamicDashboard
  DynamicJsonBuffer dashboard_root;
  JsonArray& dashboard_array = dashboard.serialize(dashboard_root);
  JsonArray& sensor_data = dashboard_array[1]["data"];

/*
  Serial.println("--------------------------------------");
  Serial.println("dynamicDashboard sensor/data:");
  sensor_data.prettyPrintTo(Serial);
  Serial.println("");
  Serial.println("--------------------------------------");
*/


  //Serial.println("searching for key: " + key);
  for (auto array_element : sensor_data) {
    if (array_element.is<JsonObject &>()){
      JsonObject& sensorObj = array_element;

      for (auto obj_element : sensorObj) {
        String dashKey = String(obj_element.key);
        if (dashKey == "name"){
          String dashValue = String(obj_element.value.as<char*>());
          if (key == dashValue) return true;
        }

        //Serial.println("searched keys: " + dashKey + "|" + dashValue);
        //if (key == dashValue){
          //Serial.println("found key: " + dashValue);
        //}

      }

    }
  }

  return false;
}

  //Serial.println("--------------------------------------");
  //Serial.println("dynamicDashboard:");
  //dashboard_array.prettyPrintTo(Serial);
  //Serial.println("");

/*
  Serial.println("--------------------------------------");
  Serial.println("1Wire Sensors:");
  sensors.prettyPrintTo(Serial);
  Serial.println("");
  Serial.println("--------------------------------------");
  Serial.println("deviceCFG");
  ffs_sensors.prettyPrintTo(Serial);
  Serial.println("--------------------------------------");
  Serial.println("");
*/
/*
  Widget* w= dashboard.insertWidget("text", "sensors");
  w->name= key;
  w->caption = key;
  w->value = value;
  w->event = "~/event/device/sensors/" + key;
  id++;
*/



    //use alias name from ffs is exists
    //String alias = strAddr;
    //if (ffs_sensors.containsKey(strAddr)) alias = ffs.deviceCFG.readItem(strAddr);
    //topicQueue.put(eventPrefix + "/" + alias + " " + temp);

    //clean old dashboard!!!!
    //String na = "sensors";
    //dashboard.removeWidget(na);

    //modify Dashboard



/*
  Widget* w1= dashboard.insertWidget("text", "sensors");
  w1->name= "banane";
  w1->caption = "Obst";
  w1->value = "gelb";
  w1->event = "~/event/device/sensors/Obst";
  id++;

  //clean old dashboard!!!!

  String delName = "banane";
  dashboard.removeWidget(delName);
*/



//...............................................................................
// read BMP180
//...............................................................................
void customDevice::readBMP180(String name) {
   Adafruit_BMP085 bmp;
   String eventPrefix= "~/event/device/" + name + "/";

   bmp.begin();
   String value = "temperature " + String(bmp.readTemperature());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
   value = "pressure " + String(bmp.readPressure()/100);
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
}

//...............................................................................
// read SI7021
//...............................................................................
void customDevice::readSi7021(String name) {
   Adafruit_Si7021 si;
   String eventPrefix= "~/event/device/" + name + "/";

   si.begin();
   String value = "temperature " + String(si.readTemperature());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
   value = "humidity " + String(si.readHumidity());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
}
