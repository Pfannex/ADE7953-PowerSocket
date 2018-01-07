#include "WIFI.h"
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
bool WIFI::start() {
  WiFiStatus = false;

  String ssid = ffs.cfg.readItem("wifi_ssid");
  String psk = ffs.cfg.readItem("wifi_password");
  String mode = ffs.cfg.readItem("wifi"); // off, dhcp, manual
  IPAddress address(0,0,0,0);
  IPAddress gateway(0,0,0,0);
  IPAddress netmask(0,0,0,0);
  IPAddress dns(0,0,0,0);

  if (mode == "off") {
    logging.info("WiFi is off");
    WiFi.disconnect(true);
  } else {
    WiFi.mode(WIFI_STA);
    if(mode == "manual") {
      address.fromString(ffs.cfg.readItem("wifi_ip"));
      gateway.fromString(ffs.cfg.readItem("wifi_gateway"));
      netmask.fromString(ffs.cfg.readItem("wifi_netmask"));
      dns.fromString(ffs.cfg.readItem("wifi_dns"));
      logging.info("WiFi static configuration");
      logging.debug("IP address: "+address.toString());
      logging.debug("gateway: "+gateway.toString());
      logging.debug("netmask: "+netmask.toString());
      logging.debug("DNS server: "+dns.toString());
    } else {
      logging.info("WiFi DHCP configuration");
    }

    logging.info("connecting WiFi to network with SSID " + ssid);
    /*
    if(WiFi.config(address, gateway, netmask, dns)) {
      logging.debug("WiFi static configuration applied");
    } else {
      logging.error("could not apply WiFi static configuration");
    }
    */
    WiFi.begin(ssid.c_str(), psk.c_str());
    int i = 0;
    // wait 30 seconds for connection
    while (WiFi.status() != WL_CONNECTED and i <= 30) {
      delay(1000);
      i++;
      if (!(i % 5)) {
        logging.info("still trying to connect...");
      }
    }

    if (WiFi.status() == WL_CONNECTED) {
      WiFiStatus = true;
      logging.info("local IP address: " + WiFi.localIP().toString());
      if (on_wifiConnected != nullptr)
        on_wifiConnected(); // callback event
    } else {
      logging.error("WiFi unable to connect");
      // start AP Ã¼ber Button!!! (GodMode?)
    }
  }
  return WiFiStatus;
}

//...............................................................................
//  WiFi handle connection
//...............................................................................
bool WIFI::handle() {

  if (WiFi.status() == WL_CONNECTED) {
    if (!WiFiStatus) {
      WiFiStatus = true;
      if (on_wifiConnected != nullptr)
        on_wifiConnected();
    }
  }

  else {
    if (WiFiStatus) {
      WiFiStatus = false;
      if (on_wifiDisconnected != nullptr)
        on_wifiDisconnected();
    }
  }
  return WiFiStatus;
}

//...............................................................................
//  WiFi set callbacks
//...............................................................................
void WIFI::set_callback(CallbackFunction wifiConnected,
                        CallbackFunction wifiDisconnected) {
  on_wifiConnected = wifiConnected;
  on_wifiDisconnected = wifiDisconnected;
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
//  API
//...............................................................................
String WIFI::get(Topic &topic) {
  if (topic.itemIs(3, "macAddress")) {
    return macAddress();
  } else {
    return TOPIC_NO;
  }
}

//-------------------------------------------------------------------------------
//  WiFi private
//-------------------------------------------------------------------------------

//...............................................................................
//  xxx
//...............................................................................
