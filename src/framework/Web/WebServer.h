#include "framework/OmniESP/API.h"
#include "framework/Web/Auth.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "Setup.h"
#include "Updater.h"

class WebServer {
public:
  WebServer(API &api);
  void start();

private:
  int isRunning = false;
  AsyncWebServer webServer;
  AsyncWebSocket webSocket;
  API &api;

  bool shouldReboot = false;

  // authenticator
  Auth auth;
  bool checkAuthentification(AsyncWebServerRequest *request);

  // page handler
  void rootPageHandler(AsyncWebServerRequest *request);
  void authPageHandler(AsyncWebServerRequest *request);
  void apiPageHandler(AsyncWebServerRequest *request);
  /*
  void updatePageHandler1(AsyncWebServerRequest *request);
  void updatePageHandler2(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
  */
  void uploadPageHandler1(AsyncWebServerRequest *request);
  void uploadPageHandler2(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
  void notFoundPageHandler(AsyncWebServerRequest *request);


  /*
  // log error when updating firmware
  void logUpdateError();
  String getUpdateErrorString();
  */

  // variable substitution in templates
  String subst(const String &var);

  // websocket event
  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len);

  // send Topics abd log entries to websocket
  void broadcast(const String &type, const String &subtype,
                 const String &value);
  void logFunction(const String &channel, const String &msg);
  void topicFunction(const time_t, Topic &topic);
};
