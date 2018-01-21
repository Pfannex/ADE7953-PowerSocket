#include "framework/NET/WIFI.h"
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
  String ssid = ffs.cfg.readItem("wifi_ssid");
  String psk = ffs.cfg.readItem("wifi_password");
  String mode = ffs.cfg.readItem("wifi"); // off, dhcp, manual

  WiFiStatus = false;
  if (mode == "off"){
    logging.info("WiFi is off");
    WiFi.disconnect(true);
  }else{   //dhcp or manual
    WiFi.mode(WIFI_STA);  //only STA without AP!
    WiFi.begin(ssid.c_str(), psk.c_str());
  }




/*
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
      logging.debug("WiFi connection established");
      WiFiStatus = true;
      // it is ugly and unreasonable to require a connection to
      // set these parameters but it is the only thing that works;
      // see https://forum.arduino.cc/index.php?topic=460595.0
      if(WiFi.config(address, gateway, netmask, dns)) {
        logging.debug("WiFi static configuration applied");
      } else {
        logging.error("could not apply WiFi static configuration");
      }
      logging.info("local IP address: " + WiFi.localIP().toString());
      if (on_wifiConnected != nullptr)
        on_wifiConnected(); // callback event
    } else {
      logging.error("WiFi unable to connect");
      // start AP Ã¼ber Button!!! (GodMode?)
    }
  }
*/

  return WiFiStatus;
}

//...............................................................................
//  WiFi handle connection
//...............................................................................
void WIFI::handle(){
  if (WiFi.status() == WL_CONNECTED) {
    if (!WiFiStatus) {
      WiFiStatus = true;
      on_connected();
    }
  }else{
    if (WiFiStatus) {
      WiFiStatus = false;
      //start();             //restart the wifi
      on_disconnected();
    }
  }
}

//...............................................................................
//  WiFi on connection established
//...............................................................................
void WIFI::on_connected(){
  String ssid = ffs.cfg.readItem("wifi_ssid");
  String psk = ffs.cfg.readItem("wifi_password");
  String mode = ffs.cfg.readItem("wifi"); // off, dhcp, manual
  IPAddress address(0,0,0,0);
  IPAddress gateway(0,0,0,0);
  IPAddress netmask(0,0,0,0);
  IPAddress dns(0,0,0,0);

  logging.info("DHCP get local IP address: " + WiFi.localIP().toString());

  if(mode == "dhcp") {
    logging.info("WiFi DHCP configuration");
  } else {  //switch to static mode
    address.fromString(ffs.cfg.readItem("wifi_ip"));
    gateway.fromString(ffs.cfg.readItem("wifi_gateway"));
    netmask.fromString(ffs.cfg.readItem("wifi_netmask"));
    dns.fromString(ffs.cfg.readItem("wifi_dns"));
    logging.info("WiFi static configuration");
    logging.debug("IP address: "+address.toString());
    logging.debug("gateway: "+gateway.toString());
    logging.debug("netmask: "+netmask.toString());
    logging.debug("DNS server: "+dns.toString());
  }
  // it is ugly and unreasonable to require a connection to
  // set these parameters but it is the only thing that works;
  // see https://forum.arduino.cc/index.php?topic=460595.0
  if(WiFi.config(address, gateway, netmask, dns)) {
    logging.debug("WiFi configuration applied");
    //logging.info("local IP address switched to: " + WiFi.localIP().toString());
  } else {
    logging.error("could not apply WiFi configuration");
  }





/*
  if (on_wifiConnected != nullptr)
    on_wifiConnected(); // callback event
} else {
  logging.error("WiFi unable to connect");
  // start AP Ã¼ber Button!!! (GodMode?)
}
*/




  if (on_wifiConnected != nullptr) on_wifiConnected();
}

//...............................................................................
//  WiFi on connection aborted
//...............................................................................
void WIFI::on_disconnected(){


  if (on_wifiDisconnected != nullptr) on_wifiDisconnected();
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
