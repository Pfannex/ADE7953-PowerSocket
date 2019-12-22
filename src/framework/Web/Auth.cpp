#include "framework/Web/Auth.h"
#include "framework/Utils/SysUtils.h"
#include <Arduino.h>

//###############################################################################
//  Session
//###############################################################################

OmniESPSession::OmniESPSession(String username, API &api)
    : username(username), api(api) {

  // the session id should be as random as possible
  sessionId = "";
  for (int i = 0; i < 32; i++) {
    sessionId.concat(String(SysUtils::rand(256), HEX));
  }
  touch();
}

String OmniESPSession::getSessionId() { return sessionId; }

String OmniESPSession::getUsername() { return username; }

bool OmniESPSession::isExpired() {
  bool expired = millis() - lastTouch > sessionLifetime;
  if (expired) {
    api.debug("session " + sessionId + " has expired.");
  }
  return expired;
}

void OmniESPSession::touch() {
  lastTouch = millis();
  // api.debug("session "+sessionId+" renewed.");
}

//###############################################################################
//  Authentification
//###############################################################################

Auth::Auth(API &api) : api(api) {

  sessions = new SessionPtr[maxSessions];
  numSessions = 0;
}

Auth::~Auth() { delete[] sessions; }

//-------------------------------------------------------------------------------
//  Authentification public
//-------------------------------------------------------------------------------

//...............................................................................
//  reset
//...............................................................................

void Auth::reset() {
  for (int i = 0; i < numSessions; i++) {
    delete sessions[i];
  }
  numSessions = 0;
}

//...............................................................................
//  checkPassword
//...............................................................................

bool Auth::checkPassword(String username, String password) {

  String sha1hash = sha1(password); // todo
  api.info("authentificating user " + username + " with SHA1 hash " + sha1hash +
           "... ");

  String device_username = api.call("~/get/ffs/cfg/item/device_username");
  String device_password = api.call("~/get/ffs/cfg/item/device_password");

  // D(username.c_str()); D(device_username.c_str());
  // D(password.c_str()); D(device_password.c_str());
  return (username == device_username) && (password == device_password);
}

//...............................................................................
//  createSession
//...............................................................................

SessionPtr Auth::createSession(String username) {

  if (numSessions == maxSessions) {
    api.error("maximum number of sessions reached.");
    return nullptr;
  } else {
    SessionPtr session = new OmniESPSession(username, api);
    sessions[numSessions++] = session;
    api.info("session " + session->getSessionId() + " created for user " +
             session->getUsername());
    return session;
  }
};

//...............................................................................
//  deleteSession
//...............................................................................

void Auth::deleteSession(String sessionId) {

  api.info("deleting session " + sessionId);
  for (int i = 0; i < numSessions; i++) {
    if (sessions[i]->getSessionId() == sessionId) {
      delete sessions[i];
      for (int j = i + 1; j < numSessions; j++) {
        sessions[j - 1] = sessions[j];
      }
      numSessions--;
      return;
    }
  }
}

//...............................................................................
//  cleanupSessions
//...............................................................................

void Auth::cleanupSessions() {

  for (int i = numSessions - 1; i >= 0; i--) {
    if (sessions[i]->isExpired())
      deleteSession(sessions[i]->getSessionId());
  }
}

//...............................................................................
//  checkSession
//...............................................................................

SessionPtr Auth::getSession(String sessionId) {

  // check if we have a session with this ID
  // api.debug("checking session " + sessionId);
  for (int i = 0; i < numSessions; i++) {
    // String sessionIdi= sessions[i]->getSessionId();
    // sysUtils.logging.debug("session has sessionId "+sessionIdi);
    if (sessions[i]->getSessionId() == sessionId) {
      // sysUtils.logging.debug("session found.");
      return sessions[i];
    }
  }
  return nullptr;
}

//-------------------------------------------------------------------------------
//  Authentification private
//-------------------------------------------------------------------------------
