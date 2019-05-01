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
    : logging(clock), ffs(logging), clock(topicQueue), espTools(logging),
      wifi(logging, ffs), device(logging, topicQueue, ffs) {

  // callback Events
  // WiFi
  wifi.set_callback(
      std::bind(&Controller::on_wifi_init, this),
      std::bind(&Controller::on_wl_connected, this),
      std::bind(&Controller::on_wl_disconnected, this),
      std::bind(&Controller::on_wl_no_ssid_avail, this),
      std::bind(&Controller::on_ap_closed, this),
      std::bind(&Controller::on_ap_stations_connected, this),
      std::bind(&Controller::on_ap_no_stations_connected, this),
      std::bind(&Controller::on_wifi_scan_result, this, std::placeholders::_1));
}

//...............................................................................
//  API set callback
//...............................................................................
void Controller::setTopicFunction(TopicFunction topicFn) {
  topicFunction = topicFn;
}

//-------------------------------------------------------------------------------
//  start
//-------------------------------------------------------------------------------

String Controller::fwConfig() {

  char txt[128];
  sprintf(txt, "%u kiB/%u kiB%s%s", espTools.physicalMemory(),
          ffs.totalMemory(),
#ifndef NO_OTA
          ", OTA",
#else
          "",
#endif
#ifdef CLOCKTICK_MINUTE
          ", tick=min"
#else
          ", tick=sec"
#endif
  );
  return String(txt);
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
  logging.info("version: " + ffs.vers.readItem("version"));
  logging.info("date   : " + ffs.vers.readItem("date"));
  logging.info("config : " + fwConfig());

  // set config defaults
  setConfigDefaults();

  // start FTP-Server and Web-server
  startFtp();
  // topicQueue.put("~/set/webserver/state", 1);

  // start WiFi for the first time
  wifi.start();

  // startup the device
  device.start();
  logging.info("controller started");
  topicQueue.put("~/event/wifi/start"); // web server will start here
}

//-------------------------------------------------------------------------------
//  getDeviceName()
//-------------------------------------------------------------------------------
String Controller::getDeviceName() { return deviceName; }

//...............................................................................
//  handle connection
//...............................................................................
void Controller::handle() {

  wifi.handle();
  handleWifiTimout();

  ftpSrv.handleFTP();
  clock.handle();
  device.handle();

  // flash?
  if (espTools.updateRequested()) {
    String event;
    event = "~/event/esp/update begin";
    handleEvent(event);
    if (espTools.update() == TOPIC_OK) {
      event = "~/event/esp/update end";
      handleEvent(event);
      delay(1000);
      espTools.reboot();
    } else {
      event = "~/event/esp/update fail";
      handleEvent(event);
    }
  }

  // handle events
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
  time_t t = clock.now();

  // logging.debug("handling event " + topicsArgs);
  Topic topic(topicsArgs);

  // propagate event to views and the device
  viewsUpdate(t, topic);
  device.on_events(topic);

  // configMode request from devices, only if supported by device
  if (topic.modifyTopic(0) == "event/device/configMode") {
    if (topic.argIs(0, "1"))
      on_config_mode_on();
    else
      on_config_mode_off();
  }
}

//...............................................................................
//  EVENT Wifi has connected
//...............................................................................
void Controller::on_wl_connected() {
  on_wifi_state_change();
  logging.info("WiFi STA connected");
  topicQueue.put("~/event/wifi/wl_connected");
  logging.info("STA connected to SSID " + WiFi.SSID());
  wifi.logIPConfig();
  on_netConnected();
}
//...............................................................................
//  EVENT wifi has disconnected
//...............................................................................
void Controller::on_wl_disconnected() {
  on_wifi_state_change();
  logging.info("WiFi STA disconnected");
  topicQueue.put("~/event/wifi/wl_disconnected");
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
//  EVENT AP was closed
//...............................................................................
void Controller::on_ap_closed() {
  logging.info("WiFi AP closed");
  topicQueue.put("~/event/wifi/ap_closed");
  on_wifi_state_change();
}

//...............................................................................
//  EVENT AP has connected with stations
//...............................................................................
void Controller::on_ap_stations_connected() {
  logging.info("WiFi AP open with connected stations");
  topicQueue.put("~/event/wifi/ap_stations_connected");
  on_wifi_state_change();
  on_netDisconnected();
}
//...............................................................................
//  EVENT AP has opened without stations
//...............................................................................
void Controller::on_ap_no_stations_connected() {
  logging.info("WiFi AP open without connected stations");
  topicQueue.put("~/event/wifi/ap_no_stations_connected");
  on_wifi_state_change();
  // is this the right place?
  // staState = STA_DISCONNECTED;
  on_netDisconnected();
}

//...............................................................................
//  EVENT WiFi-Scan result
//...............................................................................
void Controller::on_wifi_scan_result(String result) {
  logging.info("network scan completed");
  logging.info(result);

  topicQueue.put("~/event/wifi/scanResult " + result);
}

//...............................................................................
//  EVENT LAN has connected
//...............................................................................
void Controller::on_lanConnected() {
  lanState = LAN_CONNECTED;
  logging.info("LAN connected");
  topicQueue.put("~/event/lan/connected", 1);
  on_netConnected();
}

//...............................................................................
//  EVENT lan has disconnected
//...............................................................................
void Controller::on_lanDisconnected() {
  lanState = LAN_DISCONNECTED;
  logging.info("LAN disconnected");
  topicQueue.put("~/event/lan/connected", 0);
  on_netDisconnected();
}
//...............................................................................
//  EVENT Network has connected
//...............................................................................
void Controller::on_netConnected() {
  if (netState != NET_CONNECTED) {
    netState = NET_CONNECTED;
    logging.info("network connected");
    topicQueue.put("~/event/net/connected", 1);
    startNtp();
  }
}

//...............................................................................
//  EVENT lan has disconnected
//...............................................................................
void Controller::on_netDisconnected() {
  if (wifi.getStaState() != STA_CONNECTED and lanState != LAN_CONNECTED) {
    if (netState != NET_DISCONNECTED) {
      netState = NET_DISCONNECTED;
      logging.info("network disconnected");
      topicQueue.put("~/event/net/connected", 0);
    }
  }
}

//...............................................................................
//  EVENT config mode ON
//...............................................................................
void Controller::on_config_mode_on() {
  logging.info("config mode ON");
  if (ffs.cfg.readItem("ap") != "off") {
    wifi.startSTA(false);
    wifi.startAP(true);
  }
}
//...............................................................................
//  EVENT config mode OFF
//...............................................................................
void Controller::on_config_mode_off() {
  logging.info("config mode OFF");
  if (wifi.getStaState() != STA_CONNECTED and
      wifi.getApState() == AP_OPEN_WITHOUT_STATION) {
    wifi.startAP(false);
    wifi.startSTA(true);
  }
}

//...............................................................................
//  EVENT STA timeout
//...............................................................................
void Controller::on_staTimeout() {
  if (ffs.cfg.readItem("ap") != "off") {
    logging.info("STA timeout, starting access point");
    wifi.startSTA(false);
    wifi.startAP(true);
  }
}

//...............................................................................
//  EVENT AP timeout
//...............................................................................
void Controller::on_apTimeout() {
  if (ffs.cfg.readItem("ap") != "off") {
    logging.info("AP timeout, try STA reconnect");
    wifi.startAP(false);
    wifi.startSTA(true);
  }
}

//...............................................................................
//  EVENT wifi state change
//...............................................................................

void Controller::startApTimer() {
  logging.debug("starting AP timeout timer");
  // set apTimeoutActive to false if no valid SSID is available
  apTimeoutActive = wifi.hasValidSSID() ? true : false;
  apTimeout_t = clock.nowMillis();
}

void Controller::stopApTimer() {
  logging.debug("stopping AP timeout timer");
  apTimeoutActive = false;
  topicQueue.put("~/event/device/led/setmode", 0);
}

void Controller::startStaTimer() {
  logging.debug("starting STA timeout timer");
  // set staTimeout to 0 if no valid SSID is available
  staTimeout = wifi.hasValidSSID() ? STA_TIMEOUT : 0;
  staTimeout_t = clock.nowMillis();
  staTimeoutActive = true;
}

void Controller::stopStaTimer() {
  logging.debug("stopping STA timeout timer");
  staTimeoutActive = false;
}

void Controller::on_wifi_init() { on_wifi_state_change(); }

void Controller::on_wifi_state_change() {

  sta_state_t staState = wifi.getStaState();
  ap_state_t apState = wifi.getApState();

  String staS, apS;
  switch (staState) {
  case STA_UNKNOWN:
    staS = "STA_UNKNOWN";
    break;
  case STA_CONNECTED:
    staS = "STA_CONNECTED";
    break;
  case STA_DISCONNECTED:
    staS = "STA_DISCONNECTED";
    break;
  }
  switch (apState) {
  case AP_UNKNOWN:
    apS = "AP_UNKNOWN";
    break;
  case AP_TIMEOUT:
    apS = "AP_TIMEOUT";
    break;
  case AP_OPEN_WITH_STATION:
    apS = "AP_OPEN_WITH_STATION";
    break;
  case AP_OPEN_WITHOUT_STATION:
    apS = "AP_OPEN_WITHOUT_STATION";
    break;
  case AP_CLOSED:
    apS = "AP_CLOSED";
    break;
  }

  logging.info("WiFi state changed to ("+staS+","+apS+")");

  // STA_DISCONNECTED or STA_UNKNOWN
  if (staState != STA_CONNECTED and
      (apState == AP_CLOSED or apState == AP_UNKNOWN)) {
    stopApTimer();
    startStaTimer();
  }

  // STA_CONNECTED
  if (staState == STA_CONNECTED and
      (apState == AP_CLOSED or apState == AP_UNKNOWN)) {
    stopApTimer();
    stopStaTimer();
  }

  // AP_OPEN_WITHOUT_STATION
  if (staState != STA_CONNECTED and apState == AP_OPEN_WITHOUT_STATION) {
    // change blink frequency
    topicQueue.put("~/event/device/led/setmode", 2);
    stopStaTimer();
    startApTimer();
  }

  // AP_OPEN_WITH_STATION
  if (staState != STA_CONNECTED and apState == AP_OPEN_WITH_STATION) {
    // change blink frequency
    topicQueue.put("~/event/device/led/setmode", 3);
    stopStaTimer();
    stopApTimer();
  }
}

//...............................................................................
//  API-Call EVENT MQTT connected
//...............................................................................
void Controller::on_mqtt_connected() { mqtt_state = 1; }
//...............................................................................
//  API-Call EVENT MQTT disconnected
//...............................................................................
void Controller::on_mqtt_disconnected() { mqtt_state = 0; }

//...............................................................................
//  handle WiFi timeout
//...............................................................................
void Controller::handleWifiTimout() {
  unsigned long long now = clock.nowMillis();

  if (staTimeoutActive)
    if (now - staTimeout_t > staTimeout) {
      staTimeoutActive = false;
      on_staTimeout();
    }

  if (apTimeoutActive)
    if (now - apTimeout_t > apTimeout) {
      apTimeoutActive = false;
      on_apTimeout();
    }
}

//...............................................................................
//  API
//...............................................................................

String Controller::call(Topic &topic) {

  // MQTT state information via API-call
  if (topic.modifyTopic(0) == "event/mqtt/connected") {
    if (topic.argIs(0, "1")) {
      on_mqtt_connected();
    } else {
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
      if (topic.itemIs(3, "fillDashboard")) {
        return device.fillDashboard();
      } else {
        return device.set(topic);
      }
    } else if (topic.itemIs(2, "controller")) {
      if (topic.itemIs(3, "reconnectDelay")) {
        reconnectDelay = topic.getArgAsLong(0);
        if (reconnectDelay < 1)
          reconnectDelay = 1;
        if (reconnectDelay > RECONNECT_DELAY_MAX)
          reconnectDelay = RECONNECT_DELAY_MAX;
        reconnectDelayed = 0;
        return TOPIC_OK;
      } else {
        return TOPIC_NO;
      }
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
      if(topic.itemIs(3, "flags")) {
        return fwConfig();
      } else if(topic.itemIs(3, "version")) {
        return device.getVersion();
      } else if (topic.itemIs(3, "type")) {
        return device.getType();
      } else if (topic.itemIs(3, "dashboard")) {
        return device.getDashboard();
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
  // Check MQTT state and try reconnect if necessary
  if (netState == NET_CONNECTED and mqtt_state == 0 and
      ffs.cfg.readItem("mqtt") == "on") {
    if (reconnectDelayed >= reconnectDelay) {
      logging.debug("MQTT try reconnect (delayed " +
                    String(reconnectDelayed, DEC) + " s)");
      topicQueue.put("~/event/mqtt/reconnect");
      reconnectDelayed = 0;
      reconnectDelay *= 2;
      if (reconnectDelay > RECONNECT_DELAY_MAX) {
        reconnectDelay = RECONNECT_DELAY_MAX;
      }
    } else {
      if (!reconnectDelayed) {
        logging.debug("MQTT reconnect delay is " + String(reconnectDelay, DEC) +
                      " s");
      }
      reconnectDelayed++;
    }
  } else {
    reconnectDelay = 1;
    reconnectDelayed = 0;
  }

  // topicQueue.put("~/event/timer/1sUpdate");
}

void Controller::t_short_Update() {
  // topicQueue.put("~/event/timer/shortUpdate");
  /*
     espTools.debugMem();
   */
  logging.debug("uptime: " + SysUtils::uptimeStr(clock.uptimeMillis()));
};

void Controller::t_long_Update() {
  // topicQueue.put("~/event/timer/longUpdate");
}

//-------------------------------------------------------------------------------
//  Controller private
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//  set config defaults
//-------------------------------------------------------------------------------
bool Controller::setConfigDefault(String item, String defaultValue) {
  String value = ffs.cfg.readItem(item);
  if (value == "") {
    ffs.cfg.writeItem(item, defaultValue);
    logging.info("setting " + item + " to " + defaultValue);
    return true;
  } else {
    return false;
  }
}

void Controller::setConfigDefaults() {

  // set ESP name
  bool f = setConfigDefault("device_name", "ESP8266_" + espTools.genericName());
  deviceName = ffs.cfg.readItem("device_name");
  // set other defaults
  // we need to do this the hard way because the compiler optimizes the
  // the setConfigDefault() calls away if f is already true in a f= f || ..
  // sequence
  if (setConfigDefault("device_username", USERNAME))
    f = true;
  if (setConfigDefault("device_password", PASSWORD))
    f = true;
  if (setConfigDefault("device_location", ""))
    f = true;
  if (setConfigDefault("update", "manual"))
    f = true;
  if (setConfigDefault("ap", "auto"))
    f = true;
  if (setConfigDefault("ap_ssid", deviceName))
    f = true;
  if (setConfigDefault("ap_password", APPASSWORD))
    f = true;
  if (setConfigDefault("wifi", "off"))
    f = true;
  if (setConfigDefault("lan", "off"))
    f = true;
  if (setConfigDefault("ntp", "off"))
    f = true;
  if (setConfigDefault("ftp", "off"))
    f = true;
  if (setConfigDefault("mqtt", "off"))
    f = true;
  // ensure minimum length of AP password
  String appassword = ffs.cfg.readItem("ap_password");
  if (appassword.length() < 8) {
    ffs.cfg.writeItem("ap_password", APPASSWORD);
    f = true;
  }
  if (f) {
    ffs.cfg.saveFile();
  }
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
    String username = ffs.cfg.readItem("device_username");
    String password = ffs.cfg.readItem("device_password");
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
