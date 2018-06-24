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
  staMode     = ffs.cfg.readItem("wifi"); // off, dhcp, manual
  apMode      = ffs.cfg.readItem("ap");   // on, off, auto

  logging.info("MAC address: "+macAddress());

  // some workaround for WiFi.status() showing disconnect although WiFi
  // is connected
  WiFi.disconnect(true);
  WiFi.persistent(false);
  //ESP.eraseConfig();
  WiFi.mode(WIFI_OFF);
  WiFiStatus = false;

  if (staMode=="off" and apMode=="off"){
    logging.info("WiFi staMode=off and apMode=off -> no WiFi functions available!");
  } else if (apMode=="on") {
    logging.info("WiFi apMode=on -> starting AP");
    startAP(true);
  } else if (staMode=="off" and apMode=="auto") {
    logging.info("WiFi staMode=off and apMode=auto -> starting AP");
    startAP(true);

  } else {  //staMode dhcp or auto
    startSTA();
  }

/*
  if (staMode == "off") {
    logging.info("WiFi STA-Mode is off");
  } else {               // dhcp or manual
    //WiFi.mode(WIFI_AP_STA);
    WiFi.mode(WIFI_STA);
    //WiFi.mode(WIFI_AP);
    WiFi.softAPdisconnect(true);
    if (staMode == "dhcp") {
      logging.info("WiFi DHCP configuration");
    } else { // switch to static mode
      address.fromString(ffs.cfg.readItem("wifi_ip"));
      gateway.fromString(ffs.cfg.readItem("wifi_gateway"));
      netmask.fromString(ffs.cfg.readItem("wifi_netmask"));
      dns.fromString(ffs.cfg.readItem("wifi_dns"));
      logging.info("WiFi static configuration");
      logging.debug("IP address: " + address.toString());
      logging.debug("gateway: " + gateway.toString());
      logging.debug("netmask: " + netmask.toString());
      logging.debug("DNS server: " + dns.toString());

      if (WiFi.config(address, gateway, netmask, dns)) {
        logging.debug("WiFi configuration applied");
        // logging.info("local IP address switched to: " +
        // WiFi.localIP().toString());
      } else {
        logging.error("could not apply WiFi configuration");
      }
    }

    updateStatus(WiFi.begin(ssid.c_str(), psk.c_str()));
    if (ffs.cfg.readItem("ap") == "on") {
      startAP(true);
    }
  }*/

  //return wl_status;
}

//...............................................................................
//  WiFi handle connection
//...............................................................................
void WIFI::handle() {
  updateStaStatus();
  updateApStatus();

/*
  if (updateStatus(WiFi.status())) {
    if (wl_status == WL_CONNECTED) {
      if (!WiFiStatus) {
        WiFiStatus = true;
        if (ffs.cfg.readItem("ap") != "on") {
          startAP(false);
        }
        on_connected();
      }
      //Check for Networkscan
      //if (WiFi.scanComplete() != scanStatus){
        //logging.info("WiFi-Scan: " + String(WiFi.scanComplete()));
        //scanStatus = WiFi.scanComplete();
      //}
    } else if (wl_status == WL_DISCONNECTED) {
      //if (WiFiStatus) {
        WiFiStatus = false;
        if (ffs.cfg.readItem("ap") == "auto") {
          startAP(true);
        }
        on_disconnected();
      //}
    } else if (wl_status == WL_NO_SSID_AVAIL) {
        WiFiStatus = false;
        if (ffs.cfg.readItem("ap") == "auto") {
          startAP(true);
        }
        on_disconnected();
    }
  }
  //Di("wifi status", wl_status);
*/
}

//...............................................................................
//  WiFi set callbacks
//...............................................................................
void WIFI::set_callback(CallbackFunction wl_connected,
                        CallbackFunction wl_connect_failed,
                        CallbackFunction wl_no_ssid_avail,
                        CallbackFunction ap_stations_connected,
                        CallbackFunction ap_no_stations_connected) {

  on_wl_connected             = wl_connected;
  on_wl_connect_failed        = wl_connect_failed;
  on_wl_no_ssid_avail         = wl_no_ssid_avail;
  on_ap_stations_connected    = ap_stations_connected;
  on_ap_no_stations_connected = ap_no_stations_connected;
}

//...............................................................................
//  get MACAddress
//...............................................................................
String WIFI::macAddress() {
  uint8_t mac[6];
  char maddr[18];
  WiFi.macAddress(mac);
  sprintf(maddr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2],
          mac[3], mac[4], mac[5]);
  return String(maddr);
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
  } else if (topic.itemIs(3, "scan_result")){
    return scanResult();
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
*/

String WIFI::get(Topic &topic) {
  if (topic.itemIs(3, "macAddress")) {
    return macAddress();
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  start STA
//...............................................................................
void WIFI::startSTA() {
  String ssid = ffs.cfg.readItem("wifi_ssid");
  String psk  = ffs.cfg.readItem("wifi_password");
  staMode     = ffs.cfg.readItem("wifi"); // off, dhcp, manual
  apMode      = ffs.cfg.readItem("ap");   // on, off, auto
  IPAddress address(0, 0, 0, 0);
  IPAddress gateway(0, 0, 0, 0);
  IPAddress netmask(0, 0, 0, 0);
  IPAddress dns(0, 0, 0, 0);

  //if (ssid != "") validSSID = true; else validSSID = false;
  validSSID = ssid != "" ? true : false;

  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);

  if (staMode == "dhcp") {
    logging.info("WiFi staMode=dhcp");
  } else { // switch to static mode
    address.fromString(ffs.cfg.readItem("wifi_ip"));
    gateway.fromString(ffs.cfg.readItem("wifi_gateway"));
    netmask.fromString(ffs.cfg.readItem("wifi_netmask"));
    dns.fromString(ffs.cfg.readItem("wifi_dns"));
    logging.info("WiFi staMode=manual");
    logging.debug("  IP address: " + address.toString());
    logging.debug("  gateway:    " + gateway.toString());
    logging.debug("  netmask:    " + netmask.toString());
    logging.debug("  DNS server: " + dns.toString());

    if (WiFi.config(address, gateway, netmask, dns)) {
      logging.debug("WiFi configuration applied");
      // logging.info("local IP address switched to: " +
      // WiFi.localIP().toString());
    } else {
      logging.error("could not apply WiFi configuration");
    }
  }

  WiFi.begin(ssid.c_str(), psk.c_str());
}

//...............................................................................
//  start accesspoint
//...............................................................................
void WIFI::startAP(int state) {
  if (state) {
    WiFi.mode(WIFI_AP);
    IPAddress apIP(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);
    String apSSID = ffs.cfg.readItem("ap_ssid");
    String apPSK = ffs.cfg.readItem("ap_password");

    logging.info("set Accesspoint configuration");
    if (WiFi.softAPConfig(apIP, gateway, subnet)){
      logging.debug("  IP address: " + apIP.toString());
      logging.debug("  gateway:    " + gateway.toString());
      logging.debug("  subnet:     " + subnet.toString());

      if (WiFi.softAP(apSSID.c_str(), apPSK.c_str())) {
        logging.info("Accesspoint is now ON");
      } else {
        logging.error("starting Accesspoint FAILED");
      }
    } else {
      logging.error("set Accesspoint configuration FAILED");
    }
    if (on_ap_no_stations_connected != nullptr) on_ap_no_stations_connected();

    //on_ap_no_stations_connected();

/*    Serial.println(WiFi.softAPConfig(apIP, gateway, subnet));
    Serial.println(WiFi.softAP(apSSID.c_str(), apPSK.c_str()));
    Serial.println(apSSID);
    Serial.println(apPSK);
    Serial.println(WiFi.softAPIP());

    logging.info("Accesspoint is now ON");
    on_wifiConnected();*/

  } else {
    WiFi.mode(WIFI_STA);
    WiFi.softAPdisconnect(true);
    logging.info("Accesspoint is now OFF");
  }
}

//-------------------------------------------------------------------------------
//  WiFi private
//-------------------------------------------------------------------------------

//...............................................................................
//  update STA status and send callback event
//...............................................................................
bool WIFI::updateStaStatus() {
  wl_status = WiFi.status();
  if (wl_status != wl_status_old) {
    switch (wl_status) {
    case WL_NO_SHIELD:
      break;
    case WL_IDLE_STATUS:
      break;
    case WL_NO_SSID_AVAIL:
      staState = STA_DISCONNECTED;
      if (on_wl_no_ssid_avail != nullptr) on_wl_no_ssid_avail();
      break;
    case WL_SCAN_COMPLETED:
      break;
    case WL_CONNECTED:
      staState = STA_CONNECTED;
      apState  = AP_CLOSED;
      if (on_wl_connected != nullptr) on_wl_connected();
      break;
    case WL_CONNECT_FAILED:
      staState = STA_DISCONNECTED;
      if (on_wl_connect_failed != nullptr) on_wl_connect_failed();
      break;
    case WL_CONNECTION_LOST:
      break;
    case WL_DISCONNECTED:
      break;
    default:
      break;
    }
    wl_status_old = wl_status;
    return true;
  } else {
    return false;
  }
}
//...............................................................................
//  update AP status and send callback event
//...............................................................................
bool WIFI::updateApStatus() {
  if (WiFi.softAPIP().toString() != "0.0.0.0") {
    apStationsCount = WiFi.softAPgetStationNum();
    //set status
    if (apStationsCount > 0) {
       ap_status = AP_STATIONS_CONNECTED;
       apState   = AP_OPEN_WITH_STATION;
       staState  = STA_DISCONNECTED;
   } else {
       ap_status = AP_NO_STATIONS_CONNECTED;
       apState   = AP_OPEN_WITHOUT_STATION;
       staState  = STA_DISCONNECTED;
     }
     //report status
     if (ap_status != ap_status_old) {
       if (apStationsCount > 0){
         apState = AP_OPEN_WITH_STATION;
         if (on_ap_stations_connected != nullptr) on_ap_stations_connected();
       } else {
         apState = AP_OPEN_WITHOUT_STATION;
         if (on_ap_no_stations_connected != nullptr) on_ap_no_stations_connected();
       }
       ap_status_old = ap_status;
       return true;
     } else { //do nothing
       return false;
     }

  } else {
    apState = AP_CLOSED;
    return false;
  }

}

//...............................................................................
//  scan WiFi for SSIDs
//...............................................................................
String WIFI::scanWifi() {
  logging.info("start Networkscan");
  // WiFi.scanNetworks will return the number of networks found
  WiFi.scanDelete();
  WiFi.scanNetworksAsync(std::bind(&WIFI::scanResult, this));
  return "running";
}

String WIFI::scanResult(){
  int n = WiFi.scanComplete();
  logging.info("found " + String(n) + " SSIDs");

  String json = "[";
  for (int i = 0; i < n; ++i){
    if(i) json += ",";
    json += "{";
    json += "\"rssi\":"+String(WiFi.RSSI(i));
    json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
    json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
    json += ",\"channel\":"+String(WiFi.channel(i));
    json += ",\"secure\":"+String(WiFi.encryptionType(i));
    json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
    json += "}";
  }
  json += "]";

  logging.info(json);
  return json;
  //-> ~/event/wifi/scanresult json
}
