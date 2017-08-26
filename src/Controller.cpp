#include "Controller.h"

//###############################################################################
//  Controller
//###############################################################################
Controller::Controller()
    : logging(clock), i2c(logging), ffs(logging), clock(), espTools(logging),
      wifi(logging, ffs), gpio(logging) {

  // callback Events
  // WiFi
  wifi.set_callback(std::bind(&Controller::on_wifiConnected, this),
                    std::bind(&Controller::on_wifiDisconnected, this));
}

//...............................................................................
//  API set callback
//...............................................................................
void Controller::set_callback(Topic_CallbackFunction viewsUpdate) {
  on_viewsUpdate = viewsUpdate;
}

//-------------------------------------------------------------------------------
//  start
//-------------------------------------------------------------------------------
void Controller::start() {

  //
  // bootstrapping
  //

  // start the clock
  clock.start();

  // enable the logging subsystem
  logging.start();

  // start esp tools
  espTools.start();

  // fire up the FFS
  ffs.mount();

  // set ESP name on first start ever
  if (ffs.cfg.readItem("device_name") == "") {
    String deviceName = "ESP8266_" + espTools.genericName();
    logging.info("setting device name " + deviceName + " for the first time");
    ffs.cfg.writeItem("device_name", deviceName);
    logging.info("setting access point SSID " + deviceName +
                 " for the first time");
    ffs.cfg.writeItem("ap_ssid", deviceName);
    ffs.cfg.saveFile();
  }

  // start the network connections
  if (startConnections()) {

    if (ffs.cfg.readItem("ntp") == "on") {
      // start the clock with NTP updater
      String ntpServer = ffs.cfg.readItem("ntp_serverip");
      char txt[128];
      sprintf(txt, "starting NTP client for %s", ntpServer.c_str());
      logging.info(txt);
      clock.start(ntpServer.c_str(), SUMMER_TIME, NTP_UPDATE_INTERVAL);
    } else {
      logging.info("NTP client is off");
    }
  }

  startPeriphery();

  logging.info("controller started");
}

//...............................................................................
//  handle connection
//...............................................................................
void Controller::handle() {

  if (!wifi.handle()) {
    wifi.start();
  }
  gpio.handle();

  /*
  if (wifi.handle()) {
    if (!mqtt.handle()) {
      Serial.println("MQTT has disconnected!");
      delay(1000);
      mqtt.start();
    }
    webif.handle();
  } else {
    Serial.println("WiFi has disconnected!");
    wifi.start();
  }*/
}

//...............................................................................
//  EVENT Wifi has connected
//...............................................................................
void Controller::on_wifiConnected() { logging.info("WiFi has connected"); }

//...............................................................................
//  EVENT wifi has disconnected
//...............................................................................
void Controller::on_wifiDisconnected() {
  logging.info("WiFi has disconnected");
}

//...............................................................................
//  API
//...............................................................................

String Controller::call(Topic &topic) {

  // set
  if (topic.itemIs(1, "set")) {
    if (topic.itemIs(2, "ffs")) {
      return ffs.set(topic);
    } else if (topic.itemIs(2, "clock")) {
      return clock.set(topic);
    } else if (topic.itemIs(2, "esp")) {
      return espTools.set(topic);
    } else {
      return TOPIC_NO;
    }
    // get
  } else if (topic.itemIs(1, "get")) {
    if (topic.itemIs(2, "ffs")) {
      return ffs.get(topic);
    } else if (topic.itemIs(2, "clock")) {
      return clock.get(topic);
    } else if (topic.itemIs(2, "esp")) {
      return espTools.get(topic);
    } else if (topic.itemIs(2, "wifi")) {
      return wifi.get(topic);
    } else {
      return TOPIC_NO;
    }
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  idle timer
//...............................................................................
void Controller::t_1s_Update() {}

void Controller::t_short_Update() {
  clock.update();
  espTools.debugMem();
  viewsUpdate();
}

void Controller::t_long_Update() {}

//-------------------------------------------------------------------------------
//  Controller private
//-------------------------------------------------------------------------------

//...............................................................................
//  Start WiFi Connection
//...............................................................................
bool Controller::startConnections() {

  logging.info("starting network connections");
  bool result = wifi.start();
  if (result) {

    /*
    i2c.lcd.println("DHCP-IP:", ArialMT_Plain_10, 31);
    i2c.lcd.println(WiFi.localIP().toString(), ArialMT_Plain_16, 41);
    */
  }
  return result;
}

//...............................................................................
//  Start Periphery
//...............................................................................
void Controller::startPeriphery() {
  logging.info("starting periphery");
  i2c.start();
  gpio.start();
}

//...............................................................................
//  update Views
//...............................................................................
void Controller::viewsUpdate(){

  logging.debug("-> Controller::viewsUpdate()");
  Topic tmpTopic("Node52/test", "Hello World!");
  if (on_viewsUpdate != nullptr) on_viewsUpdate(tmpTopic);
}
