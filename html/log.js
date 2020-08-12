"use strict";

var debug = 1;
var log = 1;

// ------------------------------------------------------------------------

debugmsg("Debugging enabled.")

// ------------------------------------------------------------------------
// browser console log and debug
// ------------------------------------------------------------------------

function debugmsg(msg) {
  if (debug) {
    console.log("DEBUG: " + msg);
  }
}

function logmsg(msg) {
  if (log) {
    console.log("LOG  : " + msg);
  }
}

