#include "Controller.h"

//###############################################################################
//  Controller
//###############################################################################

/*
 * The controller has two main entry points: call() and handleEvent().
 * call() is called from the API and calls get or set functions in the
 * devices. The devices may react with events which they put in the
 * topicQueue to be handled by handleEvent(). Other spontaneously
 * received information from the devices are also pout in the topicQueue.
 * handleEvent() dispatches the events to the views via viewsUpdate().
 * This routine also contains the particular logic of the firmware:
 * depending on the event received, an action is carried out, i.e. a
 * function of a device is called.
 */

//...............................................................................
//  constructor
//...............................................................................
Controller::Controller()
    : logging(clock), i2c(logging), ffs(logging), clock(topicQueue),
      espTools(logging), wifi(logging, ffs), gpio(logging, topicQueue) {

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

  // set ESP name
  deviceName= ffs.cfg.readItem("device_name");
  if (deviceName == "") {
    deviceName = "ESP8266_" + espTools.genericName();
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

  setLedMode();

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
  clock.handle();
  // if(topicQueue.count) logging.debug(String(topicQueue.count) + " event(s) in
  // queue");
  while (topicQueue.count) {
    String topicsArgs = topicQueue.get();
    yield();
    handleEvent(topicsArgs);
  }

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
//  mode setter
//...............................................................................

void Controller::setPowerMode(int value) {
  power = value;
  gpio.setRelayMode(power);
  setLedMode();
}

void Controller::setConfigMode(int value) {
  if (configMode == value)
    return;
  configMode = value;
  topicQueue.put("~/event/controller/configMode", configMode);
  setLedMode();
}

void Controller::setLedMode() {
  if (!configMode) {
    if (power)
      gpio.setLedMode(ON);
    else
      gpio.setLedMode(OFF);
  } else
    gpio.setLedMode(BLINK);
}

//...............................................................................
//  handle Event
//...............................................................................
void Controller::handleEvent(String &topicsArgs) {

  //
  // this is the central routine that dispatches events from devices
  // and views
  //

  logging.debug("handling event " + topicsArgs);
  // D("Controller: create Topic object");
  // Topic topic = Topic(topicsArgs);
  Topic topic(topicsArgs);

  // propagate event to views
  // D("Controller: viewsUpdate");
  viewsUpdate(topic);

  // D("Controller: business logic");
  // central business logic
  if (topic.itemIs(2, "gpio")) {
    // Dl;
    if (topic.itemIs(3, "button")) {
      //
      // events from button
      //
      // - short
      if (topic.itemIs(4, "short")) {
        if(configMode)
          setConfigMode(0);
        else
          setPowerMode(!power);
      }
      // - long
      if (topic.itemIs(4, "long"))
        setConfigMode(!configMode);
      // - idle
      // Dl;
      if (topic.itemIs(4, "idle"))
        setConfigMode(0);
    }
  }
  // D("Controller: event handled");
}

//...............................................................................
//  EVENT Wifi has connected
//...............................................................................
void Controller::on_wifiConnected() {
  logging.info("WiFi has connected");
  topicQueue.put("~/event/wifi/connected", 1);
}

//...............................................................................
//  EVENT wifi has disconnected
//...............................................................................
void Controller::on_wifiDisconnected() {
  logging.info("WiFi has disconnected");
  topicQueue.put("~/event/wifi/connected", 0);
}

//...............................................................................
//  API
//...............................................................................

String Controller::call(Topic &topic) {

  // D("Controller: begin call");
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
  // D("Controller: end call");
}

//...............................................................................
//  idle timer
//...............................................................................
void Controller::t_1s_Update() {}

void Controller::t_short_Update() {
  espTools.debugMem();
  logging.debug("uptime: "+SysUtils::uptimeStr(clock.uptime()));
};

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
void Controller::viewsUpdate(Topic &topic) {

  if (on_viewsUpdate != nullptr)
    topic.setItem(0, deviceName.c_str());
    on_viewsUpdate(topic);
}
