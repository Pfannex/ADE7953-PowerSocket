#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs)
      {

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}


//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  Device::start();
  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {
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
    //modify dashboard-------------------
    if (topic.itemIs(4, "addItem")) {
      modifyDashboard();
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
  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

//...............................................................................
// mofify dynamicDashboard
//...............................................................................
void customDevice::modifyDashboard(){
  logging.info("modify Dashboard");

  String groupName = "sensors";

  DF("before remove");
  Serial.println("removeResult = " + String(dashboard.removeWidget(groupName)));
  DF("after remove");

  Widget* w = dashboard.insertWidget("group", 1);
    w->name= groupName;
    w->caption = "Sensorenmesswerte Heizung";

  //index=10;
  for (size_t i = 0; i < index; i++) {
    Widget* w1 = dashboard.insertWidget("text", "sensors");
    w1->name = "text_" + String(i);
    w1->caption = "World";
    w1->value = "";
    w1->event = "~/event/device/sensors/";
  }
  index++;

  //Serial.println("");
  //Serial.println("");
  //Serial.println(dashboard.asJsonDocument());
  //Serial.println("");
  //Serial.println("");
  //DynamicJsonBuffer dashboard_root;
  //JsonArray& dashboard_array = dashboard.serialize(dashboard_root);

  //Serial.println("");
  //Serial.println("");
  //dashboard_array.prettyPrintTo(Serial);
  //Serial.println("");
  DF("after adding new Items");
  Serial.println("");
  Serial.println("dashboard length = " + String(dashboard.asJsonDocument().length()));

  dashboardChanged();

}
