#include "framework/Web/Dashboard.h"

//###############################################################################
//  Widget
//###############################################################################
Dashboard::Dashboard(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
                    :logging(logging), topicQueue(topicQueue), ffs(ffs){
}

//-------------------------------------------------------------------------------
//  Dashboard public
//-------------------------------------------------------------------------------
//...............................................................................
//  Dashboard set
//...............................................................................

String Dashboard::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─dashboard        (level 3)
      └─setPage        (level 4)
      └─sensors        (level 4)
        └─alias        (level 5)
  */

  logging.debug("Dashboard set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(4, "setPage")) {
    setPage(topic.getArgAsLong(0));
    //~/set/device/dashboard/setPage 0
    return TOPIC_OK;
  } else if (topic.itemIs(4, "sensors")){
    if (topic.itemIs(5, "alias")) {
      setAlias(topic.getItem(6), topic.getArg(0));
      return TOPIC_OK;
    }else{
      return TOPIC_NO;
    }
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  Dashboard get
//...............................................................................

String Dashboard::get(Topic &topic) {
  /*
  ~/get
  └─device             (level 2)
    └─power            (level 3)
  */

  logging.debug("Dashboard get topic " + topic.topic_asString());

/*
  if (topic.getItemCount() != 4) // ~/get/device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    topicQueue.put("~/event/device/power", power);
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }*/
  return TOPIC_NO;
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void Dashboard::on_events(Topic &topic) {

  if (topic.modifyTopic(0) == "event/wifi/connected"){
    //lcd.println(WiFi.localIP().toString(), 0);
    //mcpGPIO.mcp.digitalWrite(8, true);
  }

}

//...............................................................................
// set Dashboard
//...............................................................................
void Dashboard::setPage(int page){

  switch(page){
  case 0:
    page_main();
    break;
    case 1:
      page_test();
      break;
    case 2:
      //ffs.webCFG.root = page_config();
      page_config();
      break;
    case 3:
      page_editSensornames();
      break;

  default:
    page_main();
  }
  //ffs.webCFG.saveFile();
  topicQueue.put("~/event/ui/dashboardChanged");
}

//...............................................................................
// page_menue
//...............................................................................
void Dashboard::fillMenue(JsonObject& root){

//Menue GRID
  Widget cg1("cg1", "controlgroup");
    cg1.caption   = "Menü";
    cg1.direction = "horizontal";
    cg1.action    = "~/set/device/dashboard/setPage";
    cg1.event     = "~/event/device/radio";

  cg1.fillObject(root);
    //DATA
    JsonArray& cg1_data = root.createNestedArray("data");
    JsonObject& m1 = cg1_data.createNestedObject();
      m1["label"] = "Main";
      m1["value"] = 0;
    JsonObject& m2 = cg1_data.createNestedObject();
      m2["label"] = "Test";
      m2["value"] = 1;
    JsonObject& m3 = cg1_data.createNestedObject();
      m3["label"] = "OmniESP.json";
      m3["value"] = 2;
}

//...............................................................................
// page_Config
//...............................................................................
String Dashboard::page_config(){
//get the OmniESP.json
  DynamicJsonBuffer rootCFG_jsonBuffer;
  JsonObject& rootCFG = rootCFG_jsonBuffer.parseObject(ffs.cfg.root);

//page_config root json
  DynamicJsonBuffer root_jsonBuffer;
  JsonArray& root = root_jsonBuffer.createArray();
  //add Menue
  JsonObject& root_menue = root.createNestedObject();
  fillMenue(root_menue);

//create gridObjects
  JsonObject& grid = root.createNestedObject();
    grid["type"] = "grid";
    grid["name"] = "grid1";

//create grid
  //fill GridData
  JsonArray& gridData = grid.createNestedArray("data");
  int i = 0;
  for (auto &element : rootCFG) {
    JsonArray& gridDataRow = gridData.createNestedArray();
    //column 1 "key"
    JsonObject& keyObject = gridDataRow.createNestedObject();
      keyObject["type"]     = "text";
      keyObject["name"]     = "c1" + String(i);
      keyObject["caption"]  = element.key;
      keyObject["readonly"] =  1;
      //keyObject["event"]    =  "";
      //keyObject["prefix"]   =  "";
      //keyObject["suffix"]   =  "";
    //column 1 "key"

/*
    JsonObject& valueObject = gridDataRow.createNestedObject();
      valueObject["type"]     = "text";
      valueObject["name"]     = "c2" + String(i);
      valueObject["caption"]  = element.key;
      valueObject["readonly"] =  1;
      //valueObject["event"]    =  "";
      //valueObject["prefix"]   =  "";
      //valueObject["suffix"]   =  ""; */
    i++;
  }

//return main_page rootArray as String
  //String rootStr;
  ffs.webCFG.root = "";
  root.printTo(ffs.webCFG.root);
  root.prettyPrintTo(Serial);
  Serial.println(ffs.webCFG.root);
  return "";//rootStr;
}


/*
  //fill GridData
  JsonArray& gridData = grid.createNestedArray("data");
  JsonArray& gridDataRow = gridData.createNestedArray();

  for (int i = 0; i < rootCFG.size(); i++) {
    gridDataRow[i].
  }

  int j = 0;
  for (auto &element : rootCFG) {
    //column 1 "key"
    JsonObject& keyObject = gridDataRow.createNestedObject();
      keyObject["type"]     = "text";
      keyObject["name"]     = "r" + String(j);
      keyObject["readonly"] =  1;
      keyObject["event"]    =  "";
      keyObject["prefix"]   =  String(element.key);
      keyObject["suffix"]   =  "";

    String strKey = element.key;
    String strValue = element.value;

    j++;
  }*/


//...............................................................................
// page_test
//...............................................................................
String Dashboard::page_test(){
//page_main root json
  DynamicJsonBuffer root_jsonBuffer;
  JsonArray& root = root_jsonBuffer.createArray();
  //add Menue
  JsonObject& root_menue = root.createNestedObject();
  fillMenue(root_menue);

//widget 1 GROUP
  JsonObject& w1_root = root.createNestedObject();
  Widget w1("Widget_1", "group");
    w1.caption   = "Sensors";
  w1.fillObject(w1_root);

//DATA
  JsonArray& w1_data = w1_root.createNestedArray("data");
  //widget 1 in GROUP
  JsonObject& w1_data_w1 = w1_data.createNestedObject();
    Widget w2("Widget_2", "text");
      w2.caption   = "Time 2";
      w2.readonly  = 1;
      w2.event     = "~/event/clock/time";
      w2.inputtype = "datetime";
    w2.fillObject(w1_data_w1);

  //widget 2 in GROUP
  JsonObject& w1_data_w2 = w1_data.createNestedObject();
    Widget w3("Widget_3", "button");
      w3.caption   = "Switch";
    w3.fillObject(w1_data_w2);

//return config_page rootArray as String
  ffs.webCFG.root = "";
  root.printTo(ffs.webCFG.root);
  //root.prettyPrintTo(Serial);
  Serial.println(ffs.webCFG.root);
  return "";//rootStr;


}

//...............................................................................
// page_main
//...............................................................................
String Dashboard::page_main(){
  //page_main root json
  DynamicJsonBuffer root_jsonBuffer;
  JsonArray& root = root_jsonBuffer.createArray();
  //add Menue
  JsonObject& root_menue = root.createNestedObject();
  fillMenue(root_menue);

  //GROUP1
  JsonObject& root_group1 = root.createNestedObject();
  Widget group1("group1", "group");
    group1.caption   = "Simple Switch Leitstand";
  group1.fillObject(root_group1);
    //DATA
    JsonArray& root_group1_data = root_group1.createNestedArray("data");

      //Time in GROUP
      JsonObject& root_group1_data_clock = root_group1_data.createNestedObject();
      Widget clock1("clock", "text");
        clock1.caption   = "Time";
        clock1.readonly  = 1;
        clock1.event     = "~/event/clock/time";
        clock1.inputtype = "datetime";
      clock1.fillObject(root_group1_data_clock);

      //LED in GROUP
      JsonObject& root_group1_data_led = root_group1_data.createNestedObject();
      Widget led("led", "checkbox");
        led.caption   = "LED";
        led.readonly  = 1;
        led.event     = "~/event/device/led";
      led.fillObject(root_group1_data_led);

      //SUBGROUP in GROUP
      JsonObject& root_group1_data_group2 = root_group1_data.createNestedObject();
        Widget group2("group2", "group");
        group2.caption   = "Power Control";
      group2.fillObject(root_group1_data_group2);
        //DATA
        JsonArray& root_group1_data_group2_data = root_group1_data_group2.createNestedArray("data");

          //powercontrol in SUBGROUP
          JsonObject& root_group1_data_group2_data_power = root_group1_data_group2_data.createNestedObject();
          Widget power("power", "controlgroup");
            power.caption   = "Power";
            power.action    = "~/set/device/power";
            power.event     = "~/event/device/power";
          power.fillObject(root_group1_data_group2_data_power);
            //DATA
            JsonArray& root_group1_data_group2_data_power_data = root_group1_data_group2_data_power.createNestedArray("data");
            JsonObject& d1 = root_group1_data_group2_data_power_data.createNestedObject();
              d1["label"] = "Off";
              d1["value"] = 0;
            JsonObject& d2 = root_group1_data_group2_data_power_data.createNestedObject();
              d2["label"] = "On";
              d2["value"] = 1;

          //powerbutton
          JsonObject& root_group1_data_group2_data_powerToggle = root_group1_data_group2_data.createNestedObject();
          Widget bu_toggle("toggle", "button");
            bu_toggle.caption   = "Toggle";
            bu_toggle.action     = "~/set/device/toggle";
          bu_toggle.fillObject(root_group1_data_group2_data_powerToggle);

  //GROUP3 Sensors
  JsonObject& root_group3 = root.createNestedObject();
  Widget group3("group3", "group");
    group3.caption   = "Sensoren";
  group3.fillObject(root_group3);
    //DATA
    JsonArray& root_group3_data = root_group3.createNestedArray("data");

    //get config.json "sensors"
    DynamicJsonBuffer deviceCFG_jsonBuffer;
    JsonObject& deviceCFG = deviceCFG_jsonBuffer.parseObject(ffs.deviceCFG.root);
    JsonObject& deviceCFG_sensors = deviceCFG["sensors"];
    //iterate the sensors
    for (auto &element : deviceCFG_sensors) {
      JsonObject& sensorData = deviceCFG_sensors[element.key];
      const char* xevent = sensorData["name"];
      String event = String(xevent);
      if (event == "") event = element.key;

      JsonObject& sensor = root_group3_data.createNestedObject();
      Widget sensors(String(element.key), "text");
        sensors.caption   = event;
        sensors.readonly  = 1;
        sensors.event     = "~/event/device/sensors/" + String(element.key);
        sensors.inputtype = "text";
      sensors.fillObject(sensor);

    }

    //add EDIT button
    JsonObject& edit = root_group3_data.createNestedObject();
    Widget bu_edit("edit", "button");
      bu_edit.caption   = "EDIT Sensornames";
      bu_edit.action     = "~/set/device/dashboard/setPage 3";
    bu_edit.fillObject(edit);

//return main_page rootArray as String
  ffs.webCFG.root = "";
  root.printTo(ffs.webCFG.root);
  //root.prettyPrintTo(Serial);
  //Serial.println(ffs.webCFG.root);
  return "";//rootStr;

}

//...............................................................................
// page_EditSensornames
//...............................................................................
String Dashboard::page_editSensornames(){
//page_main root json
  DynamicJsonBuffer root_jsonBuffer;
  JsonArray& root = root_jsonBuffer.createArray();

  //get config.json "sensors"
  DynamicJsonBuffer deviceCFG_jsonBuffer;
  JsonObject& deviceCFG = deviceCFG_jsonBuffer.parseObject(ffs.deviceCFG.root);
  JsonObject& deviceCFG_sensors = deviceCFG["sensors"];
  //iterate the sensors
  for (auto &element : deviceCFG_sensors) {
    //JsonObject& sensorData = deviceCFG_sensors[element.key];
    //const char* xevent = sensorData["name"];
    //String event = String(xevent);
    //if (event == "") event = element.key;

    JsonObject& sensor = root.createNestedObject();
    Widget sensors(String(element.key), "text");
      sensors.caption   = String(element.key);
      sensors.readonly  = 0;
      //sensors.event     = "~/event/device/sensors/" + String(element.key);
      sensors.action     = "~/set/device/dashboard/sensors/alias/" + String(element.key);
      sensors.inputtype = "text";
    sensors.fillObject(sensor);

  }

  //add RETURN button
  JsonObject& back = root.createNestedObject();
  Widget bu_back("edit", "button");
    bu_back.caption   = "RETURN";
    bu_back.action     = "~/set/device/dashboard/setPage 0";
  bu_back.fillObject(back);

//return config_page rootArray as String
  ffs.webCFG.root = "";
  root.printTo(ffs.webCFG.root);
  //root.prettyPrintTo(Serial);
  //Serial.println(ffs.webCFG.root);
  return "";//rootStr;
}


//...............................................................................
// Sensors count change
//...............................................................................
void Dashboard::addSensor(String newSensorID, String type){
//get the device config
  DynamicJsonBuffer deviceCFG_jsonBuffer;
  JsonObject& deviceCFG = deviceCFG_jsonBuffer.parseObject(ffs.deviceCFG.root);
  JsonObject& deviceCFG_sensors = deviceCFG["sensors"];

  //loop with array
  if (!deviceCFG_sensors.containsKey(newSensorID)) {
    JsonObject& deviceCFG_sensors_new = deviceCFG_sensors.createNestedObject(newSensorID);
    deviceCFG_sensors_new["type"] = type;
    deviceCFG_sensors_new["name"] = "";
  }

//return main_page rootArray as String
  //String rootStr;
  ffs.deviceCFG.root = "";
  deviceCFG.printTo(ffs.deviceCFG.root);
  //deviceCFG.prettyPrintTo(Serial);

  //topicQueue.put("~/event/ui/dashboardChanged");
  setPage(0);
}

//...............................................................................
// Sensors set Alias
//...............................................................................
void Dashboard::setAlias(String serial, String alias){
//get the device config
  DynamicJsonBuffer deviceCFG_jsonBuffer;
  JsonObject& deviceCFG = deviceCFG_jsonBuffer.parseObject(ffs.deviceCFG.root);
  JsonObject& deviceCFG_sensors = deviceCFG["sensors"];
  JsonObject& deviceCFG_sensor = deviceCFG_sensors[serial];
    deviceCFG_sensor["name"] = alias;



//return main_page rootArray as String
  //String rootStr;
  ffs.deviceCFG.root = "";
  deviceCFG.printTo(ffs.deviceCFG.root);
  deviceCFG.prettyPrintTo(ffs.deviceCFG.root);
}
