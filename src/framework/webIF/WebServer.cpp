#include "WebServer.h"

//###############################################################################
//  WebServer
//###############################################################################

// https://github.com/me-no-dev/ESPAsyncWebServer#espasyncwebserver

//-------------------------------------------------------------------------------
//  WebServer public
//-------------------------------------------------------------------------------

WebServer::WebServer(API &api)
    : api(api), webServer(WEBSERVERPORT), webSocket("/ws"), auth(api) {

  // static pages
  //webServer.serveStatic("/lib/jquery.js", SPIFFS, "/web/lib/jquery.js",
  //                      "max-age=86400");
  webServer.serveStatic("/lib/jquery.mobile.js", SPIFFS,
                        "/web/lib/jquery.mobile.js", "max-age=86400");
  webServer.serveStatic("/css/jquery.mobile.css", SPIFFS,
                        "/web/css/jquery.mobile.css", "max-age=86400");
  webServer.serveStatic("/omniesp.js", SPIFFS,
                        "/web/omniesp.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/css/images/ajax-loader.gif", SPIFFS,
                        "/web/css/images/ajax-loader.gif", "max-age=86400");

  // dynamic pages
  webServer.on("/", HTTP_ANY, std::bind(&WebServer::rootPageHandler, this,
                                        std::placeholders::_1));
  webServer.on("/auth.html", HTTP_ANY, std::bind(&WebServer::authPageHandler,
                                                 this, std::placeholders::_1));
  webServer.on("/api.html", HTTP_ANY, std::bind(&WebServer::apiPageHandler,
                                                this, std::placeholders::_1));

  // firmware update; the first function handles the request, the second
  // function is executed for every chunk of the uploaded file
  /*
  webServer.on(
      "/update.html", HTTP_POST,
      std::bind(&WebServer::updatePageHandler1, this, std::placeholders::_1),
      std::bind(&WebServer::updatePageHandler2, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6));
  */

  // firmware upload; the first function handles the request, the second
  // function is executed for every chunk of the uploaded file
  webServer.on(
      "/update.html", HTTP_POST,
      std::bind(&WebServer::uploadPageHandler1, this, std::placeholders::_1),
      std::bind(&WebServer::uploadPageHandler2, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6));

  // administrative pages
  webServer.onNotFound(
      std::bind(&WebServer::notFoundPageHandler, this, std::placeholders::_1));

  // websocket handlet
  webSocket.onEvent(std::bind(&WebServer::onEvent, this, std::placeholders::_1,
                              std::placeholders::_2, std::placeholders::_3,
                              std::placeholders::_4, std::placeholders::_5,
                              std::placeholders::_6));
  webServer.addHandler(&webSocket);

  // register log function
  api.registerLogFunction(std::bind(&WebServer::logFunction, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2));

  // register Topic function
  api.registerTopicFunction(std::bind(&WebServer::topicFunction, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));


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
  isRunning = true;
/*
  // register log function
  api.registerLogFunction(std::bind(&WebServer::logFunction, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2));

  // register Topic function
  api.registerTopicFunction(std::bind(&WebServer::topicFunction, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
*/
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

  String topicStr = "~/";
  topicStr += topic.modifyTopic(0);
  if (topicStr == "~/event/net/connected"){
    if (topic.getArgAsLong(0)){   //true
      start();
    }else{  //false
      api.info("Webserver is offline");
      isRunning = false;
    }
  }

  if (isRunning){
    String type("event");
    String subtype("");
    String msg = topic.asString();
    broadcast(type, subtype, msg);
  }
}

//...............................................................................
//  checkAuthentification
//...............................................................................
bool WebServer::checkAuthentification(AsyncWebServerRequest *request) {

  // check for cookie
  if (request->hasHeader("Cookie")) {
    String cookie = request->header("Cookie");
    api.debug("client provided cookie: " + cookie);
    int p = cookie.indexOf("sessionId=");
    if (p < 0) {
      api.debug("no sessionId provided.");
      return false;
    }
    // check if cookie corresponds to active session
    String sessionId = cookie.substring(p + 10, p + 74);
    api.debug("sessionId " + sessionId + " provided.");
    SessionPtr session = auth.getSession(sessionId);
    if (!session) {
      // api.debug("no session found.");
      return false;
    }
    if (session->isExpired()) {
      api.debug("session expired.");
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
//  template handlung
//...............................................................................

String WebServer::subst(const String &var) {

  if (var == "TITLE")
    return (F(TITLE));
  else if (var == "MACADDRESS")
    return api.call("~/get/wifi/macAddress");
  else if (var == "DEVICENAME")
    return api.call("~/get/ffs/cfg/item/device_name");
  else if (var == "FIRMWARE")
      return DEVICETYPE " " DEVICEVERSION;
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

    api.info("request authenticated.");
    request->send(SPIFFS, "/web/ui.html", String(), false,
                  std::bind(&WebServer::subst, this, std::placeholders::_1));
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
//http://192.168.1.99/api.html?call=Foo/bar%201,2,3

  String call = request->arg("call");
  if (call == "") {
    api.error("webserver refusing empty API call");
    return;
  }
  // log call only in DEBUG mode, it could contain sensitive information
  api.debug("webserver handling API call "+call);
  if (checkAuthentification(request)) {

    Topic tmpTopic(call);

    String result = api.call(tmpTopic);
    // contains sensitive information, log only in debug mode
    api.debug("returning " + result);
    request->send(200, "text/plain", result);
  } else {
    api.debug("client is not authenticated.");
    request->send(401, "text/plain", ""); // 401 Unauthorized
  }
}

/*
//...............................................................................
//  updatePageHandler
//...............................................................................

String WebServer::getUpdateErrorString() {
  uint8_t error = Update.getError();
  if (error == UPDATE_ERROR_OK) {
    return ("No Error");
  } else if (error == UPDATE_ERROR_WRITE) {
    return ("Flash Write Failed");
  } else if (error == UPDATE_ERROR_ERASE) {
    return ("Flash Erase Failed");
  } else if (error == UPDATE_ERROR_READ) {
    return ("Flash Read Failed");
  } else if (error == UPDATE_ERROR_SPACE) {
    return ("Not Enough Space");
  } else if (error == UPDATE_ERROR_SIZE) {
    return ("Bad Size Given");
  } else if (error == UPDATE_ERROR_STREAM) {
    return ("Stream Read Timeout");
  } else if (error == UPDATE_ERROR_MD5) {
    return ("MD5 Check Failed");
  } else if (error == UPDATE_ERROR_FLASH_CONFIG) {
    return ("Flash config wrong");
  } else if (error == UPDATE_ERROR_NEW_FLASH_CONFIG) {
    return ("new Flash config wrong");
  } else if (error == UPDATE_ERROR_MAGIC_BYTE) {
    return ("Magic byte is wrong, not 0xE9");
    //  } else if (error == UPDATE_ERROR_BOOTSTRAP) {
    //    return("Invalid bootstrapping state, reset ESP8266 before updating");
  } else {
    return ("Unknown error");
  }
}

void WebServer::logUpdateError() { api.error(getUpdateErrorString()); }

// this is the handler for the actual request
// it is executed when the POST operation has terminated
// the POST operation is handled by updatePageHandler2
void WebServer::updatePageHandler1(AsyncWebServerRequest *request) {
  shouldReboot = !Update.hasError();
  AsyncWebServerResponse *response = request->beginResponse(
      200, "text/plain", shouldReboot ? "ok" : getUpdateErrorString());
  response->addHeader("Connection", "close");
  request->send(response);
}

// this function is called for every chunk of the file which is uploaded
// in the POST operation
void WebServer::updatePageHandler2(AsyncWebServerRequest *request,
                                   String filename, size_t index, uint8_t *data,
                                   size_t len, bool final) {

  // index is 0: we receive the first chunk of the file during uploading,
  // begin the update
  if (!index) {
    api.info("updating firmware from file " + filename);
    Update.runAsync(true);
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace)) {
      logUpdateError();
      Update.printError(Serial);
    }
  }
  // for every chunk (including the first one at index 0): write it to the
  // updater as long as no error occured
  if (!Update.hasError()) {
    if (Update.write(data, len) != len) {
      logUpdateError();
    }
  }
  // when the POST operation is completed, end the update
  if (final) {
    if (Update.end(true)) {
      api.info("firmware update successful (" + String(index + len) +
               " bytes)");
    } else {
      logUpdateError();
    }
  }
}

*/

//...............................................................................
//  uploadPageHandler
//...............................................................................

// this is the handler for the actual request
// it is executed when the POST operation has terminated
// the POST operation is handled by uploadPageHandler2
void WebServer::uploadPageHandler1(AsyncWebServerRequest *request) {
}

// this function is called for every chunk of the file which is uploaded
// in the POST operation
void WebServer::uploadPageHandler2(AsyncWebServerRequest *request,
                                   String filename, size_t index, uint8_t *data,
                                   size_t len, bool final) {

}
//...............................................................................
//  all other requests
//...............................................................................

void WebServer::notFoundPageHandler(AsyncWebServerRequest *request) {
  request->send(404);
}
