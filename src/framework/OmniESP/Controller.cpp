#include "framework/OmniESP/Controller.h"

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

//-------------------------------------------------------------------------------
//  Controller public
//-------------------------------------------------------------------------------

//...............................................................................
//  constructor
//...............................................................................
Controller::Controller()
    : logging(clock), ffs(logging), clock(topicQueue),
      espTools(logging), wifi(logging, ffs), device(logging, topicQueue, ffs) {

  // callback Events
  // WiFi
  wifi.set_callback(std::bind(&Controller::on_wifiConnected, this),
                    std::bind(&Controller::on_wifiDisconnected, this));
}

//...............................................................................
//  API set callback
//...............................................................................
void Controller::setTopicFunction(TopicFunction topicFn) {
  topicFunction= topicFn;
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
  logging.info(SysUtils::fullVersion());

  // start esp tools
  espTools.start();

  // fire up the FFS
  ffs.mount();
  logging.info("tarball creation date "+SysUtils::tarballVersion());

  // set config defaults
  setConfigDefaults();

/*
  // set ESP name
  deviceName = ffs.cfg.readItem("device_name");
  if (deviceName == "") {
    deviceName = "ESP8266_" + espTools.genericName();
    logging.info("setting device name " + deviceName + " for the first time");
    ffs.cfg.writeItem("device_name", deviceName);
    ffs.cfg.saveFile();
  }
  // set accesspoint name
  if (ffs.cfg.readItem("ap_ssid") == "") {
    String apName = "ESP8266_" + espTools.genericName();
    logging.info("setting access point SSID " + apName +
                 " for the first time");
    ffs.cfg.writeItem("ap_ssid", apName);
    ffs.cfg.saveFile();
  }

  // set sane defaults
  String userName = ffs.cfg.readItem("device_username");
  if(userName == "") {
    logging.info("setting user name and password to defaults (" USERNAME "/" PASSWORD ")");
    ffs.cfg.writeItem("device_username", USERNAME);
    ffs.cfg.writeItem("device_password", PASSWORD);
    ffs.cfg.saveFile();
  }
*/

  // start WiFi for the first time
  wifi.start();

  // startup the device
  device.start();
  logging.info("controller started");
}

//-------------------------------------------------------------------------------
//  getDeviceName()
//-------------------------------------------------------------------------------
String Controller::getDeviceName() {
  return deviceName;
}

//...............................................................................
//  handle connection
//...............................................................................
void Controller::handle() {

  //if (!wifi.handle()) {
  //  wifi.start();
  //}
  wifi.handle();  //check wifi-status continuous and start if offline
  ftpSrv.handleFTP();
  clock.handle();
  device.handle();

  // flash?
  if(espTools.updateRequested()) {
    espTools.update();
  }

  //handle events
  while (topicQueue.count) {
    String topicsArgs = topicQueue.get();
    yield();
    handleEvent(topicsArgs);
  }
}

//...............................................................................
//  handle Event
//...............................................................................
void Controller::handleEvent(String &topicsArgs) {

  //
  // this is the central routine that dispatches events from devices
  // and views
  //
  time_t t= clock.now();

  //logging.debug("handling event " + topicsArgs);
  Topic topic(topicsArgs);

  // propagate event to views and the device
  viewsUpdate(t, topic);
  device.on_events(topic);

}

//...............................................................................
//  EVENT Wifi has connected
//...............................................................................
void Controller::on_wifiConnected() {
  logging.info("WiFi has connected");
  topicQueue.put("~/event/wifi/connected", 1);
  on_netConnected();
}

//...............................................................................
//  EVENT wifi has disconnected
//...............................................................................
void Controller::on_wifiDisconnected() {
  logging.info("WiFi has disconnected");
  topicQueue.put("~/event/wifi/connected", 0);
  on_netDisconnected();
}

//...............................................................................
//  EVENT LAN has connected
//...............................................................................
void Controller::on_lanConnected() {
  logging.info("LAN has connected");
  topicQueue.put("~/event/lan/connected", 1);
  on_netConnected();
}

//...............................................................................
//  EVENT lan has disconnected
//...............................................................................
void Controller::on_lanDisconnected() {
  logging.info("LAN has disconnected");
  topicQueue.put("~/event/lan/connected", 0);
  on_netDisconnected();
}
//...............................................................................
//  EVENT Network has connected
//...............................................................................
void Controller::on_netConnected() {
  if (ffs.cfg.readItem("ntp") == "on") {
    // start the clock with NTP updater
    String ntpServer = ffs.cfg.readItem("ntp_serverip");
    char txt[128];
    sprintf(txt, "starting NTP client for %.127s", ntpServer.c_str());
    logging.info(txt);
    // TODO: to have a configurable time zone, a set of timezones needs to
    // be defined in Clock.h. The right pair of TimeChangeRules need to
    // be determined based on what's in the configuration and set in
    // Clock's tz variable.
    clock.start(ntpServer.c_str(), NO_TIME_OFFSET, NTP_UPDATE_INTERVAL);
  } else {
    logging.info("NTP client is off");
  }

  // add FTP to web-config!
  //if (ffs.cfg.readItem("ftp") == "on") {
    String username= ffs.cfg.readItem("device_username");
    String password= ffs.cfg.readItem("device_password");
    logging.info("starting FTP-Server");
    ftpSrv.begin(username, password);

    //ftpSrv.begin(ffs.cfg.readItem("ftp_username"),
    //             ffs.cfg.readItem("ftp_password"));
  //} else {
    //logging.info("FTP-Server is off");
  //}

  logging.info("Network connection established");
  topicQueue.put("~/event/net/connected", 1);
}

//...............................................................................
//  EVENT lan has disconnected
//...............................................................................
void Controller::on_netDisconnected() {
  //if LAN is presend
  //check if LAN AND WiFi are disconnected!!
  logging.info("Network connection aborted");
  topicQueue.put("~/event/net/connected", 0);
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
    } else if (topic.itemIs(2, "wifi")) {
      return wifi.set(topic);
    } else if (topic.itemIs(2, "device")) {
      return device.set(topic);
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
    } else if (topic.itemIs(2, "device")) {
      return device.get(topic);
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

//-------------------------------------------------------------------------------
//  set config defaults
//-------------------------------------------------------------------------------
bool Controller::setConfigDefault(String item, String defaultValue) {
    String value= ffs.cfg.readItem(item);
    if(value == "") {
          ffs.cfg.writeItem(item, defaultValue);
          logging.info("setting "+item+" to "+defaultValue);
          return true;
    } else {
      return false;
    }
}

void Controller::setConfigDefaults() {

  // set ESP name
  bool f= setConfigDefault("device_name", "ESP8266_" + espTools.genericName());
  deviceName= ffs.cfg.readItem("device_name");
  // set other defaults
  f= f ||
    setConfigDefault("device_username", USERNAME) ||
    setConfigDefault("device_password", PASSWORD) ||
    setConfigDefault("update", "manual") ||
    setConfigDefault("ap", "auto") ||
    setConfigDefault("ap_ssid", deviceName) ||
    setConfigDefault("ap_password", APPASSWORD) ||
    setConfigDefault("wifi", "off") ||
    setConfigDefault("lan", "off") ||
    setConfigDefault("ntp", "off") ||
    setConfigDefault("mqtt", "off");
  // ensure minimum length of AP password
  String appassword= ffs.cfg.readItem("ap_password");
  if(appassword.length()< 8) {
    ffs.cfg.writeItem("ap_password", APPASSWORD);
    f= true;
  }
  if(f) { ffs.cfg.saveFile(); }

}

//...............................................................................
//  Start WiFi Connection
//...............................................................................
bool Controller::startConnections() {

  logging.info("starting network connections");
  bool result = wifi.start();
  if (result) {

    //start after wifi is connected

  }
  return result;
}

//...............................................................................
//  update Views
//...............................................................................
void Controller::viewsUpdate(time_t t, Topic &topic) {

  if (topicFunction != nullptr)
    // this is done in the API topic.setItem(0, deviceName.c_str());
    topicFunction(t, topic);
}
