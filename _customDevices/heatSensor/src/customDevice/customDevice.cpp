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
  modifySensorConfiguration(json);
}
void customDevice::on_SensorData(String json) {
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

  ffs.deviceCFG.writeItem(key, value);
  ffs.deviceCFG.saveFile();
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
  }
}

//...............................................................................
//modify ffs & dashboard
//...............................................................................
void customDevice::modifySensorConfiguration(String json){
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
  for (auto kv : sensors) {
    String key = String(kv.key);
    String value = String(kv.value.as<char*>());

    //add new Sensor to ffs
    if (!ffs_sensors.containsKey(key)){
      ffs_sensors[key] = "NEW_" + key;    //add new key
    }
  }
  ffs.deviceCFG.root = "";
  ffs_sensors.printTo(ffs.deviceCFG.root);
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
  dashboardChanged();
}

//...............................................................................
// search for key in dynamicDashboard
//...............................................................................
bool customDevice::containsKey(String key){
  //syncronize dynamicDashboard
  DynamicJsonBuffer dashboard_root;
  JsonArray& dashboard_array = dashboard.serialize(dashboard_root);
  JsonArray& sensor_data = dashboard_array[1]["data"];

  for (auto array_element : sensor_data) {
    if (array_element.is<JsonObject &>()){
      JsonObject& sensorObj = array_element;

      for (auto obj_element : sensorObj) {
        String dashKey = String(obj_element.key);
        if (dashKey == "name"){
          String dashValue = String(obj_element.value.as<char*>());
          if (key == dashValue) return true;
        }
      }
    }
  }

  return false;
}

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
