
#include <Arduino.h>

#include "Debug.h"
#include "WebSocket.h"
#include "Topic.h"

/*
WEBSocket::WEBSocket(API &api) : api(api) {

  webSocket.begin();
  // https://latedev.wordpress.com/2012/08/06/using-stdbind-for-fun-and-profit/
  webSocketEvent f = std::bind(&WEBSocket::webSocketEvent, this,
std::placeholders::_4);
  webSocket.onEvent(f);
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

void websocket_on_pubWEBIF(Topic &topic) {
  String msg= topic.asString();
  webSocket.broadcastTXT(msg);
    //api.controller.logging.debug("-> WEBIF::on_pubWEBIF()");
  //api.controller.logging.debug(topic.asString());
}

void websocket_handle() { webSocket.loop(); }

void websocket_start(API& api) {
  api.set_callbackWEBSocket(websocket_on_pubWEBIF);
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
