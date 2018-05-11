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
wl_status_t WIFI::start() {
  String ssid = ffs.cfg.readItem("wifi_ssid");
  String psk = ffs.cfg.readItem("wifi_password");
  String mode = ffs.cfg.readItem("wifi"); // off, dhcp, manual

  IPAddress address(0, 0, 0, 0);
  IPAddress gateway(0, 0, 0, 0);
  IPAddress netmask(0, 0, 0, 0);
  IPAddress dns(0, 0, 0, 0);

  // some workaround for WiFi.status() showing disconnect although WiFi
  // is connected
  WiFi.disconnect(true);
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);

  WiFiStatus = false;
  if (mode == "off") {
    logging.info("WiFi is off");
  } else {               // dhcp or manual
    WiFi.mode(WIFI_STA); // only STA without AP!
    if (mode == "dhcp") {
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
  }

  return wl_status;
}

//...............................................................................
//  WiFi handle connection
//...............................................................................
void WIFI::handle() {

  if (updateStatus(WiFi.status())) {
    if (wl_status == WL_CONNECTED) {
      if (!WiFiStatus) {
        WiFiStatus = true;
        on_connected();
      }
    } else {
      if (WiFiStatus) {
        WiFiStatus = false;
        on_disconnected();
      }
    }
  }
  //Di("wifi status", wl_status);
}

//...............................................................................
//  WiFi on connection established
//...............................................................................
void WIFI::on_connected() {
  logging.info("get local IP address: " + WiFi.localIP().toString());

  if (on_wifiConnected != nullptr)
    on_wifiConnected();
}

//...............................................................................
//  WiFi on connection aborted
//...............................................................................
void WIFI::on_disconnected() {

  if (on_wifiDisconnected != nullptr)
    on_wifiDisconnected();
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
//  update status
//...............................................................................

bool WIFI::updateStatus(wl_status_t status) {
  String s;
  if (status != wl_status) {
    switch (status) {
    case WL_NO_SHIELD:
      s = "WL_NO_SHIELD";
      break;
    case WL_IDLE_STATUS:
      s = "WL_IDLE_STATUS";
      break;
    case WL_NO_SSID_AVAIL:
      s = "WL_NO_SSID_AVAIL";
      break;
    case WL_SCAN_COMPLETED:
      s = "WL_SCAN_COMPLETED";
      break;
    case WL_CONNECTED:
      s = "WL_CONNECTED";
      break;
    case WL_CONNECT_FAILED:
      s = "WL_CONNECT_FAILED";
      break;
    case WL_CONNECTION_LOST:
      s = "WL_CONNECTION_LOST";
      break;
    case WL_DISCONNECTED:
      s = "WL_DISCONNECTED";
      break;
    default:
      s = "WL_UNKNOWN";
      break;
    }
    wl_status = status;
    logging.info("wifi status changed to " + s);
    return true;
  } else {
    return false;
  }
}
