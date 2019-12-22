#include "WebServer.h"

//###############################################################################
//  WebServer
//###############################################################################

// https://github.com/me-no-dev/ESPAsyncWebServer#espasyncwebserver

//-------------------------------------------------------------------------------
//  WebServer public
//-------------------------------------------------------------------------------

WebServer::WebServer(API &api)
    : api(api), webServer(WEBSERVERPORT), webSocket("/ws"), auth(api),
      tarball(DEFAULTTARBALL) {

  // static pages
  /*
     webServer.serveStatic("/lib/jquery.js", SPIFFS, "/web/lib/jquery.js",
                        "max-age=86400");
     webServer.serveStatic("/lib/jquery.mobile.js", SPIFFS,
                        "/web/lib/jquery.mobile.js", "max-age=86400");
   */
  webServer.serveStatic("/css/jquery.mobile.css", SPIFFS,
                        "/web/css/jquery.mobile.css", "max-age=86400");
  webServer.serveStatic("/omniesp.js", SPIFFS, "/web/omniesp.js",
                        "max-age=86400");
  webServer.serveStatic("/css/images/ajax-loader.gif", SPIFFS,
                        "/web/css/images/ajax-loader.gif", "max-age=86400");
  webServer.serveStatic("/css/images/OmniESP.png", SPIFFS,
                        "/web/css/images/OmniESP.png", "max-age=86400");

  // dynamic pages
  webServer.on(
      "/", HTTP_ANY,
      std::bind(&WebServer::rootPageHandler, this, std::placeholders::_1));
  webServer.on(
      "/auth.html", HTTP_ANY,
      std::bind(&WebServer::authPageHandler, this, std::placeholders::_1));
  webServer.on(
      "/api.html", HTTP_ANY,
      std::bind(&WebServer::apiPageHandler, this, std::placeholders::_1));

  // firmware upload; the first function handles the request, the second
  // function is executed for every chunk of the uploaded file
  webServer.on(
      "/upload.html", HTTP_POST,
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

  // get variables for templates
  macAddress = api.call("~/get/wifi/macAddress");
  deviceName = api.call("~/get/ffs/cfg/item/device_name");
  deviceLocation = api.call("~/get/ffs/cfg/item/device_location");
  deviceType = api.call("~/get/device/type");
  deviceVersion = api.call("~/get/device/version");
  firmwareVersion = api.call("~/get/ffs/version/item/version");
  flags = api.call("~/get/device/flags");

  // start serving requests
  webServer.begin();
  api.info("webserver started.");
  isRunning = true;
}

//...............................................................................
//   stop
//...............................................................................
void WebServer::stop() {
  // how to stop that thing?

  api.info("stopping webserver");

  // Authentificator
  auth.reset();

  // start serving requests
  // webServer.reset();
  api.info("webserver stopped");
  isRunning = false;
}

//...............................................................................
//  cleanup: purge idle websocket connections
//...............................................................................

void WebServer::cleanup() {
  webSocket.cleanupClients();
  auth.cleanupSessions();
}

//...............................................................................
//  run periodic task (every second)
//...............................................................................

void WebServer::runPeriodicTasks() {
  // Ds("runPeriodicTasks", delayedMessage.c_str());
  cleanup();
  webSocket.textAll(delayedMessage);
  delayedMessage = "";
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

String WebServer::eventToString(const String &type, const String &subtype,
                                const String &value) {

  String msg;
  String v = value;
  v.replace("\"", "\\\"");
  msg = "{\"type\":\"" + type + "\",";
  if (subtype != "")
    msg += "\"subtype\":\"" + subtype + "\",";
  msg += "\"value\":\"" + v + "\"}";
  return msg;
}

void WebServer::broadcast(const String &type, const String &subtype,
                          const String &value) {

  webSocket.textAll(eventToString(type, subtype, value));
}

void WebServer::logFunction(const String &channel, const String &msg) {
  String type("log");
  broadcast(type, channel, msg);
}

// send Topics to websocket
void WebServer::topicFunction(const time_t, Topic &topic) {

  String tail = topic.modifyTopic(0);
  // First react on events that affect us...
  String topicStr = "~/" + tail;

  // listen to wifi start
  if (topicStr == "~/event/wifi/start")
    start();

  /*
    Broadcasting events as they come in looses events if too much events come in
    at once. After some time, the websockets stops working after all.
    
    We thus collect the events and add them to the message string first as they 
    come in. The message string is broadcasted in the runPeriodicTasks() function.
    
    This limits the rate of broadcasting to the frequency of calling
    runPeriodicTasks() from the main program, usually to one broadcast
    per seconds.
  */
  if (isRunning) {
    String type("event");
    String subtype("");
    String msg = topic.asString();
    if(delayedMessage.length()< 2048)
      if(delayedMessage.length()> 0)
        delayedMessage += "\r\n";  
      delayedMessage += eventToString(type, subtype, msg);
  }
}

//...............................................................................
//  checkAuthentification
//...............................................................................
bool WebServer::checkAuthentification(AsyncWebServerRequest *request) {

#ifdef NO_AUTH
  return true;
#else
  // check for cookie
  if (request->hasHeader("Cookie")) {
    String cookie = request->header("Cookie");
    // api.debug("client provided cookie: " + cookie);
    int p = cookie.indexOf("sessionId=");
    if (p < 0) {
      api.debug("no sessionId provided.");
      return false;
    }
    // check if cookie corresponds to active session
    String sessionId = cookie.substring(p + 10, p + 74);
    // api.debug("sessionId " + sessionId + " provided.");
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
#endif
}

//...............................................................................
//  template handlung
//...............................................................................

String WebServer::subst(const String &var) {

  if (var == "TITLE")
    return (F(TITLE));
  else if (var == "MACADDRESS")
    return macAddress;
  else if (var == "DEVICENAME")
    return deviceName;
  else if (var == "DEVICELOCATION")
    return deviceLocation;
  else if (var == "TYPE")
    return deviceType;
  else if (var == "VERSION")
    return deviceVersion;
  else if (var == "FIRMWARE")
    return firmwareVersion;
  else if (var == "FLAGS")
    return flags;
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

  /*  api.info("serving root page");

     if (request->hasHeader("User-Agent")) {
      api.info("User-Agent: " + request->header("User-Agent"));
     }*/

#ifdef NO_AUTH
  bool authenticated = true;
#else
  bool authenticated = checkAuthentification(request);
#endif

  if (authenticated) {

    // String action = request->arg("action");
    // api.info("request authenticated.");
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
  // http://192.168.1.99/api.html?call=Foo/bar%201,2,3

  String call = request->arg("call");
  if (call == "") {
    api.error("webserver refusing empty API call");
    return;
  }
  if (checkAuthentification(request)) {
    Topic tmpTopic(call);
    String result = api.call(tmpTopic);

    const char *s = result.c_str();
    size_t l = strlen(s);

    AsyncWebServerResponse *response = request->beginChunkedResponse(
        "text/plain",
        [s, l](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
          size_t n = l - index;
          if (!n)
            return 0; // end of transfer
          size_t w = (n > maxLen) ? maxLen : n;
          memcpy(buffer, s + index, w);
          return w;
        });
    request->send(response);
  } else {
    api.debug("client is not authenticated.");
    request->send(401, "text/plain", ""); // 401 Unauthorized
  }
}

//...............................................................................
//  uploadPageHandler
//...............................................................................

// this is the handler for the actual request
// it is executed when the POST operation has terminated
// the POST operation is handled by uploadPageHandler2
void WebServer::uploadPageHandler1(AsyncWebServerRequest *request) {

#ifdef NO_OTA
  AsyncWebServerResponse *response =
      request->beginResponse(200, "text/plain", "Device does not support OTA.");
#else
  AsyncWebServerResponse *response = request->beginResponse(
      200, "text/plain", uploadOk ? "ok" : tarball.getLastError().c_str());
#endif
  response->addHeader("Connection", "close");
  request->send(response);
}

// this function is called for every chunk of the file which is uploaded
// in the POST operation
void WebServer::uploadPageHandler2(AsyncWebServerRequest *request,
                                   String filename, size_t index, uint8_t *data,
                                   size_t len, bool final) {

#ifndef NO_OTA
  if (!index) {
    // this is the first chunk
    // D("starting upload");
    // D("removing...");
    uploadOk = tarball.remove();
    if (uploadOk)
      uploadOk = tarball.beginWrite();
    else
      api.error(tarball.getLastError());
    // D("writing begun.");
    if (!uploadOk)
      api.error(tarball.getLastError());
    else
      api.info("uploading tarball from file " + filename);
  };
  // Di("writing...", len)
  if (uploadOk) {
    if (!tarball.write(data, len)) {
      uploadOk = false;
      api.error(tarball.getLastError());
    }
  }
  if (final) {
    // D("finished");
    api.info("tarball successfully uploaded (" + String(index + len) +
             " bytes)");
    tarball.endWrite();
  }
#endif
}

//...............................................................................
//  all other requests
//...............................................................................

void WebServer::notFoundPageHandler(AsyncWebServerRequest *request) {
  request->send(404);
}
