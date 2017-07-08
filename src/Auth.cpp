#include <Arduino.h>
#include "Auth.h"

//###############################################################################
//  Session
//###############################################################################

Session::Session(String username, SysUtils sysUtils):
  username(username),
  sysUtils(sysUtils) {

  // the session id should be as random as possible
  sessionId= "";
  for(int i= 0; i< 32; i++) {
    sessionId.concat(String(sysUtils.rand(256), HEX));
  }
  touch();
}

String Session::getSessionId() {
  return sessionId;
}

String Session::getUsername() {
  return username;
}

bool Session::isExpired() {
  bool expired= millis() - lastTouch > sessionLifetime;
  if(expired) {
    sysUtils.logging.debug("session "+sessionId+" has expired.");
  }
  return expired;
}

void Session::touch() {
  lastTouch= millis();
  sysUtils.logging.debug("session "+sessionId+" renewed.");
}

//###############################################################################
//  Authentification
//###############################################################################

Auth::Auth(SysUtils& sysUtils):
  sysUtils(sysUtils) {

  sessions= new SessionPtr[maxSessions];
  numSessions= 0;
}

Auth::~Auth() {
  delete sessions;
}

//-------------------------------------------------------------------------------
//  Authentification public
//-------------------------------------------------------------------------------

//...............................................................................
//  reset
//...............................................................................

void Auth::reset() {
  for(int i= 0; i< numSessions; i++) {
    delete sessions[i];
  }
  numSessions= 0;
}

//...............................................................................
//  checkPassword
//...............................................................................

bool Auth::checkPassword(String username, String password) {

  String sha1hash= sha1(password);
  sysUtils.logging.info("authentificating user "+username+" with SHA1 hash "+sha1hash+"... ");

  // this is a stub!
  return (username == "admin") && (password == "admin");
}

//...............................................................................
//  createSession
//...............................................................................

SessionPtr Auth::createSession(String username) {

  if(numSessions== maxSessions) {
    sysUtils.logging.error("maximum number of sessions reached.");
    return nullptr;
  } else {
    SessionPtr session= new Session(username, sysUtils);
    sessions[numSessions++]= session;
    sysUtils.logging.info("session " + session->getSessionId() +
      " created for user " + session->getUsername());
    return session;
  }

};

//...............................................................................
//  deleteSession
//...............................................................................

void Auth::deleteSession(String sessionId) {

  sysUtils.logging.info("deleting session " + sessionId);
  for(int i= 0; i< numSessions; i++) {
      if(sessions[i]->getSessionId()== sessionId) {
        delete sessions[i];
        for(int j= i+1; j< numSessions; j++) {
          sessions[j-1]= sessions[j];
        }
        numSessions--;
        return;
      }
  }
}

//...............................................................................
//  checkSession
//...............................................................................

SessionPtr Auth::getSession(String sessionId) {

  // check if we have a session with this ID
  sysUtils.logging.info("checking session " + sessionId);
  for(int i= 0; i< numSessions; i++) {
    //String sessionIdi= sessions[i]->getSessionId();
    //sysUtils.logging.debug("session has sessionId "+sessionIdi);
    if(sessions[i]->getSessionId()== sessionId) {
      //sysUtils.logging.debug("session found.");
      return sessions[i];
    }
  }
  return nullptr;
}

//-------------------------------------------------------------------------------
//  Authentification private
//-------------------------------------------------------------------------------
