#include "WebServer.h"

//###############################################################################
//  WebServer
//###############################################################################

// https://github.com/me-no-dev/ESPAsyncWebServer#espasyncwebserver-

//-------------------------------------------------------------------------------
//  WebServer public
//-------------------------------------------------------------------------------

WebServer::WebServer(API &api)
    : api(api), webServer(WEBSERVERPORT), webSocket("/ws"), auth(api) {

  // static pages
  webServer.serveStatic("/lib/jquery.js", SPIFFS, "/web/lib/jquery.js",
                        "max-age=86400");
  webServer.serveStatic("/lib/jquery.mobile.js", SPIFFS,
                        "/web/lib/jquery.mobile.js", "max-age=86400");
  webServer.serveStatic("/css/jquery.mobile.css", SPIFFS,
                        "/web/css/jquery.mobile.css", "max-age=86400");
  webServer.serveStatic("/auth.js", SPIFFS,
                        "/web/auth.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/config.js", SPIFFS,
                        "/web/config.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/panels.js", SPIFFS,
                        "/web/panels.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/console.js", SPIFFS, "/web/console.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/ui.js", SPIFFS, "/web/ui.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/css/images/ajax-loader.gif", SPIFFS,
                        "/web/css/images/ajax-loader.gif", "max-age=86400");

  // dynamic pages
  webServer.on("/", HTTP_ANY, std::bind(&WebServer::rootPageHandler, this,
                                        std::placeholders::_1));
  webServer.on("/auth.html", HTTP_ANY, std::bind(&WebServer::authPageHandler,
                                                 this, std::placeholders::_1));

  // administrative pages
  webServer.onNotFound(
      std::bind(&WebServer::notFoundPageHandler, this, std::placeholders::_1));

  // websocket handlet
  webSocket.onEvent(std::bind(&WebServer::onEvent, this, std::placeholders::_1,
                              std::placeholders::_2, std::placeholders::_3,
                              std::placeholders::_4, std::placeholders::_5,
                              std::placeholders::_6));
  webServer.addHandler(&webSocket);
}

//...............................................................................
//   start
//...............................................................................
void WebServer::start() {

  api.info("starting webserver");

  // Authentificator
  auth.reset();

  // start serving requests
  webServer.begin();
  api.info("webserver started.");

  // register log function
  api.registerLogFunction(std::bind(&WebServer::logFunction, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2));

  // register Topic function
  api.registerTopicFunction(std::bind(&WebServer::topicFunction, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
}

//-------------------------------------------------------------------------------
//  WebServer private
//-------------------------------------------------------------------------------

//...............................................................................
//  web socket event handler
//...............................................................................

void WebServer::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                        AwsEventType type, void *arg, uint8_t *data,
                        size_t len) {
  // Handle WebSocket event
  char msg[256];
  if (type == WS_EVT_CONNECT) {
    sprintf(msg, "client #%u connected to websocket", client->id());
    api.info(msg);
  } else if (type == WS_EVT_DISCONNECT) {
    sprintf(msg, "client #%u disconnected from websocket", client->id());
    api.info(msg);
  } else {
    sprintf(msg, "unhandled websocket event for client #%u", client->id());
    api.error(msg);
  }
}

//...............................................................................
//  push Topics and log entries to websocket
//...............................................................................

void WebServer::broadcast(const String &type, const String &subtype,
                          const String &value) {
  String msg;
  String v = value;
  v.replace("\"", "\\\"");
  msg = "{\"type\":\"" + type + "\",";
  if (subtype != "")
    msg += "\"subtype\":\"" + subtype + "\",";
  msg += "\"value\":\"" + v + "\"}";
  webSocket.textAll(msg);
}

void WebServer::logFunction(const String &channel, const String &msg) {
  String type("log");
  broadcast(type, channel, msg);
}

// send Topics to websocket
void WebServer::topicFunction(const time_t, Topic &topic) {
  String type("event");
  String subtype("");
  String msg = topic.asString();
  broadcast(type, subtype, msg);
}

//...............................................................................
//  checkAuthentification
//...............................................................................
bool WebServer::checkAuthentification(AsyncWebServerRequest *request) {

  // check for cookie
  if (request->hasHeader("Cookie")) {
    String cookie = request->header("Cookie");
    api.info("client provided cookie: " + cookie);
    int p = cookie.indexOf("sessionId=");
    if (p < 0) {
      api.info("no sessionId provided.");
      return false;
    }
    // check if cookie corresponds to active session
    String sessionId = cookie.substring(p + 10, p + 74);
    api.info("sessionId " + sessionId + " provided.");
    SessionPtr session = auth.getSession(sessionId);
    if (!session) {
      // api.debug("no session found.");
      return false;
    }
    if (session->isExpired()) {
      // api.debug("session expired.");
      // delete the session
      auth.deleteSession(sessionId);
      return false;
    }
    // api.debug("session found.");
    session->touch();
    return true;
  } else {
    return false;
  }
}

//...............................................................................
//  apply configuration
//...............................................................................
void WebServer::applyConfiguration(AsyncWebServerRequest *request) {

  api.info("applying configuration");
  if (request->args()) {
    String root = "{\"";
    for (int i = 0; i < request->args(); i++) {
      if (i)
        root += ",\"";
      root += request->argName(i) + "\":\"" + request->arg(i) + "\"";
    }
    root += "}";
    api.call("~/set/ffs/cfg/root " + root);
    api.call("~/set/ffs/cfg/saveFile");
  };
}

//...............................................................................
//  get configuration
//...............................................................................
String WebServer::getConfiguration() { return api.call("~/get/ffs/cfg/root"); }

//...............................................................................
//  template handlung
//...............................................................................

String WebServer::subst(const String &var) {

  if (var == "TITLE")
    return (F(TITLE));
  else if (var == "MACADDRESS")
    return api.call("~/get/wifi/macAddress");
  else if (var == "DEVICENAME")
    return api.call("~/get/ffs/cfg/item/device_name");
  else
    return F("?");
}

//...............................................................................
//
//  page handlers
//
//...............................................................................

//...............................................................................
//  rootPageHandler
//...............................................................................

void WebServer::rootPageHandler(AsyncWebServerRequest *request) {

  api.info("serving root page");

  if (request->hasHeader("User-Agent")) {
    api.info("User-Agent: " + request->header("User-Agent"));
  }

  bool authenticated = checkAuthentification(request);

  if (authenticated) {

    String action = request->arg("action");

    // reboot
    if (action == "reboot") {
      api.info("action: reboot");
      request->send(200, "text/plain", "true");
      api.call("~/set/esp/restart"); // never returns
    } else if (action == "apply") {
      api.info("action: apply config");
      // https://code.tutsplus.com/tutorials/jquery-mobile-framework-a-forms-tutorial--mobile-4500
      applyConfiguration(request);
      request->send(200, "text/plain", "true");
    } else if (action == "config") {
      api.info("action: get config");
      request->send(200, "application/json", getConfiguration());
    } else {
      // show dashboard
      api.info("request authenticated.");
      request->send(SPIFFS, "/web/ui.html", String(), false,
                    std::bind(&WebServer::subst, this, std::placeholders::_1));
    }
  } else {
    // send user to login page
    api.info("request not authenticated.");
    request->send(SPIFFS, "/web/login.html", String(), false,
                  std::bind(&WebServer::subst, this, std::placeholders::_1));
  }
}

//...............................................................................
//  authPageHandler
//...............................................................................
void WebServer::authPageHandler(AsyncWebServerRequest *request) {

  String action = request->arg("action");
  AsyncWebServerResponse *response;

  if (action == "login") {

    // --- login
    String username = request->arg("username");
    String password = request->arg("password");

    if (auth.checkPassword(username, password)) {
      // password ok
      api.info("user " + username + " authenticated.");
      SessionPtr session = auth.createSession(username);
      if (!session) {
        // could not create a session
        api.info("could not create a session.");
        response = request->beginResponse(200, "text/plain", "false");
        response->addHeader("Location", "/");
        response->addHeader("Cache-Control", "no-cache");
        request->send(response);
        return;
      }
      response = request->beginResponse(200, "text/plain", "true");
      response->addHeader("Location", "/");
      response->addHeader("Cache-Control", "no-cache");
      response->addHeader("Set-Cookie", "sessionId=" + session->getSessionId());
      request->send(response);

    } else {
      // password not ok
      api.info("user " + username + " authentification failed");
      response = request->beginResponse(200, "text/plain", "false");
      response->addHeader("Location", "/");
      response->addHeader("Cache-Control", "no-cache");
      request->send(response);
    }

  } else if (action == "logout") {

    // --- logout
    api.info("logging out");
    response = request->beginResponse(200, "text/plain", "true");
    response->addHeader("Location", "/");
    response->addHeader("Cache-Control", "no-cache");
    response->addHeader("Set-Cookie", "sessionId=");
    request->send(response);
  }
}

//...............................................................................
//  apiPageHandler
//...............................................................................

void WebServer::apiPageHandler(AsyncWebServerRequest *request) {

  String call = request->arg("call");
  api.info("webserver handling API call " + call);
  if (checkAuthentification(request)) {

    Topic tmpTopic(call);

    String result = api.call(tmpTopic);
    api.debug("returning " + result);
    request->send(200, "text/plain", result);
  } else {
    api.debug("client is not authenticated.");
    request->send(404, "text/plain", "");
  }
}

//...............................................................................
//  all other requests
//...............................................................................

void WebServer::notFoundPageHandler(AsyncWebServerRequest *request) {
  request->send(404);
}
