
#include "API.h"
#include "Setup.h"
#include <WebSocketsServer.h>

//###############################################################################
//  web sockets
//###############################################################################

/*
class WEBSocket {
public:
  WEBSocket(API& api);

  void start();
  void handle();
  // Callback Events
  // API
  void on_pubWEBSocket(Topic &topic);
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t
length);

private:
  API& api;

  WebSocketsServer webSocket = WebSocketsServer(WEBSOCKETSPORT);
  // authenticator
  //Auth auth;
  //bool checkAuthentification();

};
*/

void websocket_handle();
void websocket_start(API &api);
void websocket_onLog(const String &channel, const String &msg);
// private
void websocket_broadcast(const String &type, const String &subtype,
                         const String &value);
bool websocket_onValidateHttpHeader(String headerName, String headerValue);
