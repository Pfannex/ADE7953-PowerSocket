
#include <Arduino.h>

#include "Debug.h"
#include "Topic.h"
#include "WebSocket.h"

/*
WEBSocket::WEBSocket(API &api) : api(api) {

  webSocket.begin();
  // https://latedev.wordpress.com/2012/08/06/using-stdbind-for-fun-and-profit/
  webSocket.onEvent( std::bind(&WEBSocket::webSocketEvent, this, std::placeholders::_4) );
}

void WEBSocket::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                               size_t length) {

  char msg[256];
  switch (type) {
  case WStype_DISCONNECTED:
    sprintf(msg, "[%u] Disconnected!\n", num);
    D(msg);
    break;
  case WStype_CONNECTED: {
    IPAddress ip = webSocket.remoteIP(num);
    sprintf(msg, "[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1],
            ip[2], ip[3], payload);
    D(msg);

    // send message to client
    webSocket.sendTXT(num, "Connected");
  } break;
  case WStype_TEXT:
    sprintf(msg, "[%u] get Text: %s\n", num, payload);
    D(msg);

    // send message to client
    // webSocket.sendTXT(num, "message here");

    // send data to all connected clients
    // webSocket.broadcastTXT("message here");
    break;
  case WStype_BIN:
    sprintf(msg, "[%u] get binary length: %u\n", num, length);
    D(msg);
    // hexdump(payload, length);

    // send message to client
    // webSocket.sendBIN(num, payload, length);
    break;
  }
}

void WEBSocket::handle() { webSocket.loop(); }
*/



WebSocketsServer webSocket = WebSocketsServer(WEBSOCKETSPORT);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {

  char msg[256];
  switch (type) {
  case WStype_DISCONNECTED:
    /*
    sprintf(msg, "[%u] Disconnected!\n", num);
    D(msg)
    */;
    break;
  case WStype_CONNECTED:
    /*
    IPAddress ip = webSocket.remoteIP(num);
    sprintf(msg, "[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1],
            ip[2], ip[3], payload);
    D(msg);
    */
    break;
  case WStype_TEXT:
    /*
      sprintf(msg, "[%u] get Text: %s\n", num, payload);
      D(msg);
  */
    break;
  case WStype_BIN:
    /*
    sprintf(msg, "[%u] get binary length: %u\n", num, length);
    D(msg);
    */
    break;
  }
}

bool websocket_onValidateHttpHeader(String headerName, String headerValue) {
    D("checking header");
    D(headerName.c_str());
    D(headerValue.c_str());
    return true;
}

void websocket_broadcast(const String &type, const String &subtype,
                         const String &value) {
  String msg;
  String v = value;
  v.replace("\"", "\\\"");
  msg = "{\"type\":\"" + type + "\",";
  if (subtype != "")
    msg += "\"subtype\":\"" + subtype + "\",";
  msg += "\"value\":\"" + v + "\"}";
  webSocket.broadcastTXT(msg);
}

void websocket_on_pubWEBIF(Topic &topic) {
  String type("event");
  String subtype("");
  String msg = topic.asString();
  websocket_broadcast(type, subtype, msg);
}

void websocket_onLog(const String &channel, const String &msg) {
  String type("log");
  websocket_broadcast(type, channel, msg);
}

void websocket_handle() { webSocket.loop(); }

void websocket_start(API &api) {
  api.set_callbackWEBSocket(websocket_on_pubWEBIF);
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  const char* mandatoryHeaders[1];
  mandatoryHeaders[0]= "Cookie";
  webSocket.onValidateHttpHeader(websocket_onValidateHttpHeader, mandatoryHeaders, 1);
}
