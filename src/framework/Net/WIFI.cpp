#include "framework/Net/WIFI.h"
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

//###############################################################################
//  WiFi
//###############################################################################
WIFI::WIFI(LOGGING &logging, FFS &ffs) : logging(logging), ffs(ffs) {}

//-------------------------------------------------------------------------------
//  WiFi public
//-------------------------------------------------------------------------------

//...............................................................................
//  WiFi start connection
//...............................................................................
void WIFI::start() {

  String staMode = ffs.cfg.readItem("wifi"); // off, dhcp, manual
  String apMode = ffs.cfg.readItem("ap");    // on, off, auto
  staState = STA_UNKNOWN;
  apState = AP_UNKNOWN;

  //
  logging.info("MAC address: " + WiFi.macAddress());

  // some workaround for WiFi.status() showing disconnect although WiFi
  // is connected
  WiFi.disconnect(true);
  WiFi.persistent(false);
  // ESP.eraseConfig();
  WiFi.mode(WIFI_OFF);

  // initial status update
  if (on_wifi_init != nullptr)
    on_wifi_init();

  if (staMode == "off" and apMode == "off") {
    logging.info(
        "WiFi staMode=off and apMode=off -> no WiFi functions available!");
  } else if (apMode == "on") {
    logging.info("WiFi apMode=on -> starting AP");
    startAP(true);
  } else if (staMode == "off" and apMode == "auto") {
    logging.info("WiFi staMode=off and apMode=auto -> starting AP");
    startAP(true);
  } else { // staMode dhcp or auto
    startSTA(true);
  }
}

void WIFI::logIPConfig() {

  logging.info("  MAC address: " + WiFi.macAddress());
  logging.info("  IP address:  " + WiFi.localIP().toString());
  logging.info("  netmask:     " + WiFi.subnetMask().toString());
  logging.info("  gateway:     " + WiFi.gatewayIP().toString());
  logging.info("  DNS server:  " + WiFi.dnsIP().toString());
  logging.info("  hostname:    " + WiFi.hostname());
  logging.info("  RSSI:        " + String(WiFi.RSSI(), DEC) + " dBm");
}

//...............................................................................
//  WiFi handle connection
//...............................................................................
void WIFI::handle() {
  updateStaStatus();
  updateApStatus();
}

//...............................................................................
//  WiFi set callbacks
//...............................................................................
void WIFI::set_callback(CallbackFunction wifi_init,
                        CallbackFunction wl_connected,
                        CallbackFunction wl_disconnected,
                        CallbackFunction wl_no_ssid_avail,
                        CallbackFunction ap_closed,
                        CallbackFunction ap_stations_connected,
                        CallbackFunction ap_no_stations_connected,
                        String_CallbackFunction wifi_scan_result) {

  on_wifi_init = wifi_init;
  on_wl_connected = wl_connected;
  on_wl_disconnected = wl_disconnected;
  on_wl_no_ssid_avail = wl_no_ssid_avail;
  on_ap_closed = ap_closed;
  on_ap_stations_connected = ap_stations_connected;
  on_ap_no_stations_connected = ap_no_stations_connected;
  on_wifi_scan_result = wifi_scan_result;
}

//...............................................................................
//  WiFi Set
//...............................................................................
/*
~/set
└─wifi              (level 2)
  └─scan            (level 3)
*/

String WIFI::set(Topic &topic) {

  if (topic.itemIs(3, "scan")) {
    return scanWifi();
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  WiFi Get
//...............................................................................
/*
~/get
└─wifi              (level 2)
  └─macAddress      (level 3)
  └─scanResult      (level 3)
*/

String WIFI::get(Topic &topic) {
  if (topic.itemIs(3, "macAddress")) {
    return WiFi.macAddress();
  } else if (topic.itemIs(3, "scanResult")) {
    return scanResult();
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  start STA
//...............................................................................
void WIFI::startSTA(int state) {
  if (state) {
    String hostname = ffs.cfg.readItem("device_name");
    String ssid = ffs.cfg.readItem("wifi_ssid");
    String psk = ffs.cfg.readItem("wifi_password");
    String staMode = ffs.cfg.readItem("wifi"); // off, dhcp, manual

    IPAddress address(0, 0, 0, 0);
    IPAddress gateway(0, 0, 0, 0);
    IPAddress netmask(0, 0, 0, 0);
    IPAddress dns(0, 0, 0, 0);

    if (staMode == "dhcp") {
      logging.info("connecting WiFi (DHCP)");
    } else { // switch to static mode
      logging.info("connecting WiFi (static)");
      address.fromString(ffs.cfg.readItem("wifi_ip"));
      gateway.fromString(ffs.cfg.readItem("wifi_gateway"));
      netmask.fromString(ffs.cfg.readItem("wifi_netmask"));
      dns.fromString(ffs.cfg.readItem("wifi_dns"));
      logging.debug("  hostname:    " + hostname);
      logging.debug("  MAC address: " + WiFi.macAddress());
      logging.debug("  IP address:  " + address.toString());
      logging.debug("  gateway:     " + gateway.toString());
      logging.debug("  netmask:     " + netmask.toString());
      logging.debug("  DNS server:  " + dns.toString());

      if (WiFi.config(address, gateway, netmask, dns)) {
        logging.debug("WiFi configuration applied");
        // logging.info("local IP address switched to: " +
        // WiFi.localIP().toString());
      } else {
        logging.error("could not apply WiFi configuration");
      }
    }

    logging.info("try to connect to SSID: " + ssid);
    WiFi.begin(ssid.c_str(), psk.c_str());
    WiFi.hostname(hostname);
  } else {
    logging.info("disconnecting WiFi");
    WiFi.disconnect(true);
  }
  updateStaStatus();
}

//...............................................................................
//  start access point
//...............................................................................
void WIFI::startAP(int state) {
  if (state) {
    logging.info("turning access point on");
    IPAddress apIP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    String apSSID = ffs.cfg.readItem("ap_ssid");
    String apPSK = ffs.cfg.readItem("ap_password");
    logging.info("setting access point configuration");
    if (WiFi.softAPConfig(apIP, gateway, subnet)) {
      logging.debug("access point configured");
      logging.debug("  MAC address: " + WiFi.softAPmacAddress());
      logging.debug("  IP address:  " + WiFi.softAPIP().toString());
      logging.debug("  gateway:     " + gateway.toString());
      logging.debug("  subnet:      " + subnet.toString());
      logging.debug("opening access point with SSID " + apSSID + " and PSK " +
                    apPSK);
      if (WiFi.softAP(apSSID.c_str(), apPSK.c_str())) {
        logging.info("access point is now on");
      } else {
        logging.error("starting access point failed");
      }
    } else {
      logging.error("access point configuration failed");
    }

  } else {
    logging.info("turning access point off");
    WiFi.softAPdisconnect(true);
  }
  updateApStatus();
}

//...............................................................................
//  status getter
//...............................................................................

sta_state_t WIFI::getStaState() { return staState; }

ap_state_t WIFI::getApState() { return apState; }

bool WIFI::hasValidSSID() { return ffs.cfg.readItem("wifi_ssid") != ""; }

//-------------------------------------------------------------------------------
//  WiFi private
//-------------------------------------------------------------------------------

//...............................................................................
//  update STA status and send callback event
//...............................................................................
bool WIFI::updateStaStatus() {

  // our state of the station
  sta_state_t staStateOld = staState;

  // internal state of WiFi according to Arduino library
  wl_status_t wlStateOld = wlState;
  wlState = WiFi.status();

  if (wlState != wlStateOld)
    switch (wlState) {
    case WL_NO_SHIELD:
      staState = STA_DISCONNECTED;
      break;
    case WL_IDLE_STATUS:
      break;
    case WL_NO_SSID_AVAIL:
      staState = STA_DISCONNECTED;
      if (on_wl_no_ssid_avail != nullptr)
        on_wl_no_ssid_avail();
      break;
    case WL_SCAN_COMPLETED:
      break;
    case WL_CONNECTED:
      staState = STA_CONNECTED;
      break;
    case WL_CONNECT_FAILED:
      staState = STA_DISCONNECTED;
      break;
    case WL_CONNECTION_LOST:
      staState = STA_DISCONNECTED;
      break;
    case WL_DISCONNECTED:
      staState = STA_DISCONNECTED;
      break;
    default:
      break;
    }

  // derive state change of station
  bool stateChanged = (staState != staStateOld);
  if (stateChanged) {
    switch (staState) {
    case STA_CONNECTED:
      if (on_wl_connected != nullptr)
        on_wl_connected();
      break;
    case STA_DISCONNECTED:
      if (on_wl_disconnected != nullptr)
        on_wl_disconnected();
      break;
    case STA_UNKNOWN:
      // does not occur
      break;
    }
  }
  return stateChanged;
}
//...............................................................................
//  update AP status and send callback event
//...............................................................................
bool WIFI::updateApStatus() {
  ap_state_t apStateOld = apState;

  if (WiFi.softAPIP().toString() != "(IP unset)") {
    apState = WiFi.softAPgetStationNum() ? AP_OPEN_WITH_STATION
                                         : AP_OPEN_WITHOUT_STATION;
  } else {
    apState = AP_CLOSED;
  }

  bool stateChanged = (apState != apStateOld);
  if (stateChanged) {
    switch (apState) {
    case AP_CLOSED:
      if (on_ap_closed != nullptr)
        on_ap_closed();
      break;
    case AP_OPEN_WITHOUT_STATION:
      if (on_ap_no_stations_connected != nullptr)
        on_ap_no_stations_connected();
      break;
    case AP_OPEN_WITH_STATION:
      if (on_ap_stations_connected != nullptr)
        on_ap_stations_connected();
      break;
    case AP_UNKNOWN:
      // should not occur
      break;
    }
  }

  return stateChanged;
}

//...............................................................................
//  scan WiFi for SSIDs
//...............................................................................
String WIFI::scanWifi() {
  logging.info("start Networkscan");
  // WiFi.scanNetworks will return the number of networks found
  WiFi.scanDelete();
  WiFi.scanNetworksAsync(std::bind(&WIFI::on_scanWifi_complete, this));
  return "running";
}

void WIFI::on_scanWifi_complete() {
  String result = scanResult();
  if (on_wifi_scan_result != nullptr)
    on_wifi_scan_result(result);
}

String WIFI::scanResult() {
  int n = WiFi.scanComplete();
  logging.info("found " + String(n) + " SSIDs");

  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i)
      json += ",";
    json += "{";
    json += "\"rssi\":" + String(WiFi.RSSI(i));
    json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
    json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
    json += ",\"channel\":" + String(WiFi.channel(i));
    json += ",\"secure\":" + String(WiFi.encryptionType(i));
    json += ",\"hidden\":" + String(WiFi.isHidden(i) ? "true" : "false");
    json += "}";
  }
  json += "]";

  return json;
}
