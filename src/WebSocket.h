
#include <WebSocketsServer.h>
#include "Setup.h"
#include "API.h"

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
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

private:
  API& api;

  WebSocketsServer webSocket = WebSocketsServer(WEBSOCKETSPORT);
  // authenticator
  //Auth auth;
  //bool checkAuthentification();


};
*/
void websocket_handle();
void websocket_start(API& api);
