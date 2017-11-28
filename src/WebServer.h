
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "API.h"
#include "Setup.h"


class WebServer {
public:
  WebServer(API& api);

  void start();
  void handle();
  // Callback Events
  // API
  void on_pubWEBIF(Topic &topic);

private:
  AsyncWebServer webServer;
  AsyncWebSocket webSocket;
  ESP8266HTTPUpdateServer httpUpdater;
  API& api;

  // authenticator
  Auth auth;
  bool checkAuthentification();

  // number of pages served
  long numPagesServed= 0;

  // serve file with some logging
  void send(const String &description, int code, char *content_type, const String &content);

  // send a file
  void sendFile(const String &description, int code, char *content_type, const String filePath);

  // page handler
  void rootPageHandler();
  void authPageHandler();
  void apiPageHandler();
  void handleNotFound();

  // variable substitution
  String subst(String data);

  // configuration
  void applyConfiguration();
  String getConfiguration();

};
