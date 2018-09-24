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
  wifi.set_callback(std::bind(&Controller::on_wl_connected, this),
                    std::bind(&Controller::on_wl_connect_failed, this),
                    std::bind(&Controller::on_wl_no_ssid_avail, this),
                    std::bind(&Controller::on_ap_stations_connected, this),
                    std::bind(&Controller::on_ap_no_stations_connected, this),
                    std::bind(&Controller::on_wifi_scan_result, this,
                                           std::placeholders::_1)
                   );
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
  logging.info(SysUtils::fullVersion()); // framework version

  // start esp tools
  espTools.start();

  // fire up the FFS
  ffs.mount();

  // show the versioning
  logging.info("version: "+ffs.vers.readItem("version"));
  logging.info("date   : "+ffs.vers.readItem("date"));

  // set config defaults
  setConfigDefaults();

  // start FTP-Server and Web-server
  startFtp();
  //topicQueue.put("~/set/webserver/state", 1);

  // start WiFi for the first time
  wifi.start();

  // startup the device
  device.start();
  logging.info("controller started");
  topicQueue.put("~/event/wifi/start"); //web server will start here

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

  wifi.handle();
  on_wifi_state_change();
  handleWifiTimout();

  ftpSrv.handleFTP();
  clock.handle();
  device.handle();

  // flash?
  if(espTools.updateRequested()) {
    String event;
    event= "~/event/esp/update begin";
    handleEvent(event);
    if(espTools.update() == TOPIC_OK) {
      event= "~/event/esp/update end";
      handleEvent(event);
      delay(1000);
      espTools.reboot();
    } else {
      event= "~/event/esp/update fail";
      handleEvent(event);
    }
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

  // configMode request from devices, only if supported by device
  if (topic.modifyTopic(0) == "event/device/configMode"){
    if (topic.argIs(0, "1")) on_config_mode_on();
      else on_config_mode_off();
  }
}

//...............................................................................
//  EVENT Wifi has connected
//...............................................................................
void Controller::on_wl_connected() {
  on_wifi_state_change();
  logging.info("WiFi STA has connected");
  topicQueue.put("~/event/wifi/wl_connected");
  logging.info("STA connected to " + WiFi.SSID() + " | IP "
                                   + WiFi.localIP().toString());

  startNtp();
  on_netConnected();
}
//...............................................................................
//  EVENT wifi has disconnected
//...............................................................................
void Controller::on_wl_connect_failed() {
  on_wifi_state_change();
  logging.info("WiFi STA has failed to connect");
  topicQueue.put("~/event/wifi/wl_connect_failed");
  on_netDisconnected();
}
//...............................................................................
//  EVENT wifi no SSID avail
//...............................................................................
void Controller::on_wl_no_ssid_avail() {
  on_wifi_state_change();
  logging.info("WiFi STA SSID is not avail");
  topicQueue.put("~/event/wifi/no_ssid_avail");
  on_netDisconnected();
}

//...............................................................................
//  EVENT AP has connected with stations
//...............................................................................
void Controller::on_ap_stations_connected() {
  on_wifi_state_change();
  logging.info("WiFi AP open with connected stations");
  topicQueue.put("~/event/wifi/ap_stations_connected");
}
//...............................................................................
//  EVENT AP has opened without stations
//...............................................................................
void Controller::on_ap_no_stations_connected() {
  on_wifi_state_change();
  logging.info("WiFi AP open without connected stations");
  topicQueue.put("~/event/wifi/ap_no_stations_connected");

  //is this the right place?
  staState = STA_DISCONNECTED;
  on_netDisconnected();
}

//...............................................................................
//  EVENT WiFi-Scan result
//...............................................................................
void Controller::on_wifi_scan_result(String result) {
  logging.info("Networkscan done");
  logging.info(result);

  topicQueue.put("~/event/wifi/scanResult " + result);
}

//...............................................................................
//  EVENT LAN has connected
//...............................................................................
void Controller::on_lanConnected() {
  lanState = LAN_CONNECTED;
  logging.info("LAN has connected");
  topicQueue.put("~/event/lan/connected", 1);
  on_netConnected();
}

//...............................................................................
//  EVENT lan has disconnected
//...............................................................................
void Controller::on_lanDisconnected() {
  lanState = LAN_DISCONNECTED;
  logging.info("LAN has disconnected");
  topicQueue.put("~/event/lan/connected", 0);
  on_netDisconnected();
}
//...............................................................................
//  EVENT Network has connected
//...............................................................................
void Controller::on_netConnected() {
  netState = NET_CONNECTED;
  logging.info("Network connection established");
  topicQueue.put("~/event/net/connected", 1);
}

//...............................................................................
//  EVENT lan has disconnected
//...............................................................................
void Controller::on_netDisconnected() {
  if (staState != STA_CONNECTED and lanState != LAN_CONNECTED) {
    netState = NET_DISCONNECTED;
    logging.info("Network connection aborted");
    topicQueue.put("~/event/net/connected", 0);
  }
}

//...............................................................................
//  EVENT config mode ON
//...............................................................................
void Controller::on_config_mode_on() {
  logging.info("config mode ON");
  if (wifi.apMode != "off"){
    wifi.startAP(true);
  }
}
//...............................................................................
//  EVENT config mode OFF
//...............................................................................
void Controller::on_config_mode_off() {
  logging.info("config mode OFF");
  if (staState != STA_CONNECTED and apState == AP_OPEN_WITHOUT_STATION) {
    wifi.startAP(false);
    wifi.startSTA();
  }
}

//...............................................................................
//  EVENT STA timeout
//...............................................................................
void Controller::on_staTimeout() {
  if (wifi.apMode != "off") {
    logging.info("STA timeout starting access point");
    wifi.startAP(true);
  }
}

//...............................................................................
//  EVENT AP timeout
//...............................................................................
void Controller::on_apTimeout() {
  if (wifi.apMode == "auto") {
    logging.info("AP timeout try STA reconnect");
    wifi.startAP(false);
    wifi.startSTA();
  }
}

//...............................................................................
//  EVENT wifi state change
//...............................................................................
void Controller::on_wifi_state_change() {
  if (staState != wifi.staState or apState != wifi.apState) {
    staState = wifi.staState;
    apState  = wifi.apState;

//STA_DISCONNECTED
    if (staState == STA_DISCONNECTED and apState == AP_CLOSED){
      logging.info("WiFi state changed to STA_DISCONNECTED");
      logging.debug("  STA_DISCONNECTED");
      logging.debug("  AP_CLOSED");

      //turn off apTimeout
      apTimeoutActive = false;
      //set staTimeout to 0 if no valid SSID is available
      staTimeout = wifi.validSSID ? STA_TIMEOUT : 0;
      //start the STA Timer
      staTimeout_t = clock.nowMillis();
      staTimeoutActive = true;
      logging.debug("start staTimeoutTimer");
    }

//STA_CONNECTED
    if (staState == STA_CONNECTED and apState == AP_CLOSED){
      logging.info("WiFi state changed to STA_CONNECTED");
      logging.debug("  STA_CONNECTED");
      logging.debug("  AP_CLOSED");

      //turn off staTimeout
      staTimeoutActive = false;
    }

//AP_OPEN_WITHOUT_STATION
    if (staState == STA_DISCONNECTED and apState == AP_OPEN_WITHOUT_STATION){
      //change blink frequency
      topicQueue.put("~/event/device/led/setmode", 2);

      logging.info("WiFi state changed to AP_OPEN_WITHOUT_STATION");
      logging.debug("  STA_DISCONNECTED");
      logging.debug("  AP_OPEN_WITHOUT_STATION");

      //turn off staTimeout
      staTimeoutActive = false;
      //set apTimeoutActive to false if no valid SSID is available
      apTimeoutActive = wifi.validSSID ? true : false;
      //start the AP Timer
      apTimeout_t = clock.nowMillis();
      logging.debug("start apTimeoutTimer");
    }

//AP_OPEN_WITH_STATION
    if (staState == STA_DISCONNECTED and apState == AP_OPEN_WITH_STATION){
      //change blink frequency
      topicQueue.put("~/event/device/led/setmode", 3);

      logging.info("WiFi state changed to AP_OPEN_WITH_STATION");
      logging.debug("  STA_DISCONNECTED");
      logging.debug("  AP_OPEN_WITH_STATION");

      //turn off apTimeout
      apTimeoutActive = false;
    }
  }
}

//...............................................................................
//  API-Call EVENT MQTT connected
//...............................................................................
void Controller::on_mqtt_connected() {
  mqtt_state = 1;
}
//...............................................................................
//  API-Call EVENT MQTT disconnected
//...............................................................................
void Controller::on_mqtt_disconnected() {
  mqtt_state = 0;
}

//...............................................................................
//  handle WiFi timeout
//...............................................................................
void Controller::handleWifiTimout() {
  unsigned long long now = clock.nowMillis();

  if (staTimeoutActive)
    if (now - staTimeout_t > staTimeout){
      staTimeoutActive = false;
      on_staTimeout();
    }

  if (apTimeoutActive)
    if (now - apTimeout_t > apTimeout){
      apTimeoutActive = false;
      on_apTimeout();
    }
}

//...............................................................................
//  API
//...............................................................................

String Controller::call(Topic &topic) {

  // MQTT state information via API-call
  if (topic.modifyTopic(0) == "event/mqtt/connected"){
    if (topic.argIs(0, "1")){
      on_mqtt_connected();
    }else{
      on_mqtt_disconnected();
    }
  }

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
      if(topic.itemIs(3, "version")) {
        return device.getVersion();
      } else if (topic.itemIs(3, "type")) {
        return device.getType();
      } else {
        return device.get(topic);
      }
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
void Controller::t_1s_Update() {
  //Check MQTT state and try reconnect if necessary
  if (netState == NET_CONNECTED and mqtt_state == 0 and ffs.cfg.readItem("mqtt") == "on"){
    logging.debug("MQTT try reconnect");
    topicQueue.put("~/event/mqtt/reconnect");
  }

  //topicQueue.put("~/event/timer/1sUpdate");
}

void Controller::t_short_Update() {
  //topicQueue.put("~/event/timer/shortUpdate");
  /*
    espTools.debugMem();
  */
  logging.debug("uptime: "+SysUtils::uptimeStr(clock.uptimeMillis()));
};

void Controller::t_long_Update() {
  //topicQueue.put("~/event/timer/longUpdate");
}

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
  // we need to do this the hard way because the compiler optimizes the
  // the setConfigDefault() calls away if f is already true in a f= f || ..
  // sequence
  if(setConfigDefault("device_username", USERNAME)) f= true;
  if(setConfigDefault("device_password", PASSWORD)) f= true;
  if(setConfigDefault("update", "manual")) f= true;
  if(setConfigDefault("ap", "auto")) f= true;
  if(setConfigDefault("ap_ssid", deviceName)) f= true;
  if(setConfigDefault("ap_password", APPASSWORD)) f= true;
  if(setConfigDefault("wifi", "off")) f= true;
  if(setConfigDefault("lan", "off")) f= true;
  if(setConfigDefault("ntp", "off")) f= true;
  if(setConfigDefault("ftp", "off")) f= true;
  if(setConfigDefault("mqtt", "off")) f= true;
  // ensure minimum length of AP password
  String appassword= ffs.cfg.readItem("ap_password");
  if(appassword.length()< 8) {
    ffs.cfg.writeItem("ap_password", APPASSWORD);
    f= true;
  }
  if(f) { ffs.cfg.saveFile(); }

}

//...............................................................................
//  update Views
//...............................................................................
void Controller::viewsUpdate(time_t t, Topic &topic) {

  if (topicFunction != nullptr)
    // this is done in the API topic.setItem(0, deviceName.c_str());
    topicFunction(t, topic);
}

//...............................................................................
//  Start FTP-Server
//...............................................................................
bool Controller::startFtp() {

  if (ffs.cfg.readItem("ftp") == "on") {
    String username= ffs.cfg.readItem("device_username");
    String password= ffs.cfg.readItem("device_password");
    logging.info("starting FTP server");
    ftpSrv.begin(username, password);
  } else {
    logging.info("FTP server is off");
  }
}

//...............................................................................
//  Start NTP
//...............................................................................
bool Controller::startNtp() {
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
}
