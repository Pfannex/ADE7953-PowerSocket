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

/*
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
  }*/


//https://wizwiki.net/wiki/doku.php?id=products:wiz850io:start
  // GND        GND
  // GND        3V3
  // MOSI -D7   3V3
  // CLK  -D5   NC
  // CS   -D2   reset
  // INT        MISO  -D6


  /*
   * CLK  = GPIO 14 | D5
   * MISO = GPIO 12 | D6
   * MOSI = GPIO 13 | D7
   * CS   = GPIO 15 | D8
   */

  byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};

  //WARNING
  //W5500 CS has internal PullUp, GPIO15 is also used for BootMode-selection
  //with GPIO15 set to HIGH ESP starts with wrong BootMode!
  //set CS-pin to GPIO4,
  Ethernet.init(4);
  // start the Ethernet connection:
  Ethernet.begin(mac);
  //Serial.println("hardwareStatus: " + String(Ethernet.hardwareStatus()));
  //Serial.println("linkStatus    : " + String(Ethernet.linkStatus()));

  if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      // no point in carrying on, so do nothing forevermore:
      for (;;)
        ;
    }
/*
    // print your local IP address:
    Serial.print("My IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print(".");
    }
    Serial.println();
*/



  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }


  // print your local IP address:
  //Serial.print("My IP address: ");
  //Serial.println(Ethernet.localIP());

  staState = STA_CONNECTED;
  apState  = AP_CLOSED;
  if (on_wl_connected != nullptr) on_wl_connected();

}

//...............................................................................
//  WiFi handle connection
//...............................................................................
void WIFI::handle() {

  auto link = Ethernet.linkStatus();
  Serial.print("Link status: ");
  switch (link) {
    case Unknown:
      Serial.println("Unknown");
      break;
    case LinkON:
      Serial.println("ON");
      break;
    case LinkOFF:
      Serial.println("OFF");
      break;
  }

  auto hlink = Ethernet.hardwareStatus();
  Serial.print("hardware status: ");
  switch (hlink) {
    case EthernetNoHardware:
      Serial.println("EthernetNoHardware");
      break;
    case EthernetW5100:
      Serial.println("EthernetW5100");
      break;
    case EthernetW5200:
      Serial.println("EthernetW5200");
      break;
    case EthernetW5500:
      Serial.println("EthernetW5500");
      break;

  }

  delay(1000);


  //Serial.println("hardwareStatus: " + String(Ethernet.hardwareStatus()));
  //Serial.println("linkStatus    : " + String(Ethernet.linkStatus()));
  //updateStaStatus();
  //updateApStatus();
}

//...............................................................................
//  WiFi set callbacks
//...............................................................................
void WIFI::set_callback(CallbackFunction wl_connected,
                        CallbackFunction wl_connect_failed,
                        CallbackFunction wl_no_ssid_avail,
                        CallbackFunction ap_stations_connected,
                        CallbackFunction ap_no_stations_connected,
                        String_CallbackFunction wifi_scan_result ){

  on_wl_connected             = wl_connected;
  on_wl_connect_failed        = wl_connect_failed;
  on_wl_no_ssid_avail         = wl_no_ssid_avail;
  on_ap_stations_connected    = ap_stations_connected;
  on_ap_no_stations_connected = ap_no_stations_connected;
  on_wifi_scan_result         = wifi_scan_result;
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
    return macAddress();
  } else if (topic.itemIs(3, "scanResult")){
    return scanResult();
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  start STA
//...............................................................................
void WIFI::startSTA() {
/*
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

  logging.info("try to connect to SSID: " + ssid);
  WiFi.begin(ssid.c_str(), psk.c_str());
  */
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


/*
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
  }*/
  return false;
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
  WiFi.scanNetworksAsync(std::bind(&WIFI::on_scanWifi_complete, this));
  return "running";
}

void WIFI::on_scanWifi_complete(){
  String result = scanResult();
  if (on_wifi_scan_result != nullptr) on_wifi_scan_result(result);
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

  return json;
}
