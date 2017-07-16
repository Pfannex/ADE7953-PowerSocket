#include "WEBServer.h"

//###############################################################################
//  web interface
//###############################################################################

WEBIF::WEBIF(SysUtils& sysUtils):
       sysUtils(sysUtils),
       //api(api),
       webServer(80),
       auth(sysUtils) {

  httpUpdater.setup(&webServer);

  // static pages
  // we need short file names on SPIFFS, else they do not load
  webServer.serveStatic("/lib/jquery.js", SPIFFS, "/web/lib/jquery.js", "max-age=86400");
  webServer.serveStatic("/lib/jquery.mobile.js", SPIFFS, "/web/lib/jquery.mobile.js", "max-age=86400");
  webServer.serveStatic("/css/jquery.mobile.css", SPIFFS, "/web/css/jquery.mobile.css", "max-age=86400");
  webServer.serveStatic("/auth.js", SPIFFS, "/web/auth.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/config.js", SPIFFS, "/web/config.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/panels.js", SPIFFS, "/web/panels.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/ui.js", SPIFFS, "/web/ui.js" /*, "max-age=86400"*/);
  webServer.serveStatic("/css/images/ajax-loader.gif", SPIFFS, "/web/css/images/ajax-loader.gif", "max-age=86400");
  // dynamic pages
  webServer.on("/", std::bind(&WEBIF::rootPageHandler, this));
  webServer.on("/auth.html", std::bind(&WEBIF::authPageHandler, this));
  webServer.on("/api.html", std::bind(&WEBIF::apiPageHandler, this));
  webServer.onNotFound(std::bind(&WEBIF::handleNotFound, this));

}

//-------------------------------------------------------------------------------
//  web interface public
//-------------------------------------------------------------------------------
//...............................................................................
//  web interface start
//...............................................................................
void WEBIF::start() {

  sysUtils.logging.info("starting webserver... ");

  // pages served
  numPagesServed= 0;

  // Authentificator
  auth.reset();

  //here the list of headers to be recorded
  const char* headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);

  //ask server to track these headers
  webServer.collectHeaders(headerkeys, headerkeyssize );

  // start serving requests
  webServer.begin();
  sysUtils.logging.info("webserver started.");

}

//...............................................................................
//  web interface handle
//...............................................................................
void WEBIF::handle() {

   webServer.handleClient();
}

//-------------------------------------------------------------------------------
//  web interface private
//-------------------------------------------------------------------------------
//...............................................................................
//  configuration
//...............................................................................
void WEBIF::applyConfiguration() {

  sysUtils.logging.info("applying configuration...");
  for(int i= 0; i< webServer.args(); i++) {
    sysUtils.logging.debug(webServer.argName(i) + ": " + webServer.arg(i));
    //ffs.cfg.writeItem(webServer.argName(i), webServer.arg(i));
    String strName = webServer.argName(i);
    String strArg = webServer.arg(i);
    //api.call(strName, strArg);
  };
  //ffs.cfg.saveFile();

}

//...............................................................................
//  WEBIF::getConfiguration()
//...............................................................................
String WEBIF::getConfiguration() {
  //return ffs.cfg.root;
}


//...............................................................................
//  page handlers and related functions
//...............................................................................
String WEBIF::subst(String data) {

  /*
  int p1= 0, p2;
  String key;

  while((p1= data.indexOf('$DATA('), p1)>= 0) {
    p2= data.indexOf(')', p1+6);
    key= data.substring(p1+6,p2-1);
    if(key == 'MACADDRESS') {
        data= data.substring(0,p1
    }
  }*/
  data.replace("$DATA(MACADDRESS)", sysUtils.net.macAddress());
  data.replace("$DATA(DEVICEID)", String(sysUtils.esp_tools.chipID()));
  return data;

}


//...............................................................................
//  WEBIF::send
//...............................................................................
void WEBIF::send(const String &description, int code, char *content_type, const String &content) {

  numPagesServed++;
  sysUtils.logging.info("serving "+description);
  sysUtils.logging.debugMem();
  webServer.send(code, content_type, content);
}

//...............................................................................
//  WEBIF::sendFile
//...............................................................................
void WEBIF::sendFile(const String &description, int code, char *content_type, const String filePath) {
  // http://esp8266.github.io/Arduino/versions/2.0.0/doc/filesystem.html
  // https://github.com/pellepl/spiffs/wiki/Using-spiffs
  File f;

  sysUtils.logging.info("serving "+description);
  sysUtils.logging.debug("reading " + filePath + "... ");
  if (SPIFFS.exists(filePath)) {
    f = SPIFFS.open(filePath, "r");
    if (f) {
      sysUtils.logging.debug("opened.");
      webServer.send(code, content_type, subst(f.readString()));
      f.close();
    } else {
      sysUtils.logging.error("cannot open "+filePath);
    }
  } else {
      sysUtils.logging.error("file "+filePath+" does not exist.");
  }

}


//...............................................................................
//  WEBIF::checkAuthentification
//...............................................................................
bool WEBIF::checkAuthentification() {

  // check for cookie
  if(webServer.hasHeader("Cookie")) {
    String cookie = webServer.header("Cookie");
    sysUtils.logging.info("client provided cookie: "+cookie);
    int p= cookie.indexOf("sessionId=");
    if(p< 0) {
      sysUtils.logging.info("no sessionId provided.");
      return false;
    }
    // check if cookie corresponds to active session
    String sessionId= cookie.substring(p+10,p+74);
    sysUtils.logging.info("sessionId "+sessionId+" provided.");
    SessionPtr session= auth.getSession(sessionId);
    if(!session) {
      //sysUtils.logging.debug("no session found.");
      return false;
    }
    if(session->isExpired()) {
      //sysUtils.logging.debug("session expired.");
      // delete the session
      auth.deleteSession(sessionId);
      return false;
    }
    //sysUtils.logging.debug("session found.");
    session->touch();
    return true;
  } else {
    return false;
  }
}

//...............................................................................
//  WEBIF::rootPageHandler
//...............................................................................
void WEBIF::rootPageHandler() {

  sysUtils.logging.info("serving root page...");

  // Debug only
  //Serial.println("Headers:");
  //for(int i= 0; i< webServer.headers(); Serial.println(webServer.header(i++)));

  if(webServer.hasHeader("User-Agent")) {
    sysUtils.logging.info("User-Agent: "+webServer.header("User-Agent"));
  }

  bool authenticated= checkAuthentification();


  if(authenticated) {

    String action= webServer.arg("action");

    // reboot
    if(action == "reboot") {
      sysUtils.logging.info("action: reboot");
      webServer.send(200, "text/plain", "true");
      sysUtils.esp_tools.reboot(); // never returns
    }
    // config
    if(action == "apply") {
      sysUtils.logging.info("action: apply config");
      // https://code.tutsplus.com/tutorials/jquery-mobile-framework-a-forms-tutorial--mobile-4500
      applyConfiguration();
      webServer.send(200, "text/plain", "true");
    } else if(action == "config") {
      sysUtils.logging.info("action: get config");
      webServer.send(200, "application/json", getConfiguration());
    } else {
      // show dashboard
      sysUtils.logging.info("request authenticated.");
      sendFile("dashboard", 200, "text/html;charset=UTF-8", "/web/ui.html");
    }
  } else {
    // send user to login page
    sysUtils.logging.info("request not authenticated.");
    sendFile("login page", 200, "text/html;charset=UTF-8", "/web/login.html");
  }
}

//...............................................................................
//  WEBIF::authPageHandler
//...............................................................................
void WEBIF::authPageHandler() {

  String action= webServer.arg("action");

  if(action == "login") {

    // --- login
    String username= webServer.arg("username");
    String password= webServer.arg("password");

    if(auth.checkPassword(username, password)) {
      // password ok
      sysUtils.logging.info("user "+username+" authenticated.");
      SessionPtr session= auth.createSession(username);
      if(!session) {
        // could not create a session
        sysUtils.logging.info("could not create a session.");
        webServer.sendHeader("Location","/");
        webServer.sendHeader("Cache-Control","no-cache");
        webServer.send(200, "text/plain", "false");
        return;
      }
      webServer.sendHeader("Location","/");
      webServer.sendHeader("Cache-Control","no-cache");
      webServer.sendHeader("Set-Cookie","sessionId="+session->getSessionId());
      webServer.send(200, "text/plain", "true");

    } else {
      // password not ok
      sysUtils.logging.info("user "+username+" authentification failed");
      webServer.sendHeader("Location","/");
      webServer.sendHeader("Cache-Control","no-cache");
      webServer.send(200, "text/plain", "false");

    }

  } else if(action == "logout") {

    // --- logout
    sysUtils.logging.info("logging out");
    webServer.sendHeader("Location","/");
    webServer.sendHeader("Cache-Control","no-cache");
    webServer.sendHeader("Set-Cookie","sessionId=");
    webServer.send(200, "text/plain", "true");
  }

}

//...............................................................................
//  WEBIF::apiPageHandler
//...............................................................................


void WEBIF::apiPageHandler() {

  String call= webServer.arg("call");
  sysUtils.logging.info("webserver handling API call "+call);
  if(checkAuthentification()) {

    Topic tmpTopic(call);

    String result= "NIL";
    //String result=  api.call(tmpTopic);
    sysUtils.logging.debug("returning "+result);
    webServer.send(200, "text/plain", result);
  } else {
    sysUtils.logging.debug("client is not authenticated.");
    webServer.send(404, "text/plain", "");
  }
}

//...............................................................................
//  404 file not found
//...............................................................................
void WEBIF::handleNotFound() {

  sysUtils.logging.error("URI "+webServer.uri()+" not found.");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";

  for (uint8_t i = 0; i < webServer.args(); i++){
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
}
