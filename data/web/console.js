"use strict";

var consConn;

var consLastIndex = 0;
var withLog = 0;
var mustScroll= [1, 1];

function message(msg) {
  $("#message").html(msg);
}

function message_clear() {
  message("");
}

function consoleSet(n, msg) {
  $("#console" + n).html(msg);
}

function consoleScroll(n) {
  var h = $("#console" + n)[0].scrollHeight;
  //console.log("scrolling to "+h);
  $("#console" + n).scrollTop(h);
}

function consoleWriteln(n, msg) {
  var d = new Date();
  $("#console" + n).append(d.toLocaleString()+" "+msg + "<br>");
  if (mustScroll[n]) {
    consoleScroll(n);
  }
}


function consEvalContent(content) {
  var fields= JSON.parse(content);
  if(fields.type=="event") {
    consoleWriteln(1, fields.value)
  } else if (fields.type=="log") {
    consoleWriteln(2, fields.subtype+" "+fields.value)
  }
}

function consCloseConn() {
  if (!consConn)
    return;
  if (typeof consConn.close == "function")
    consConn.close();
  else if (typeof consConn.abort == "function")
    consConn.abort();
  consConn = undefined;
}

function consUpdate(evt) {
  var errstr = "Connection lost, trying to reconnect every 5 seconds.";
  var new_content = "";

  if ((typeof WebSocket == "function" || typeof WebSocket == "object") && evt &&
    evt.target instanceof WebSocket) {
    if (evt.type == 'close') {
      message(errstr);
      consCloseConn();
      setTimeout(consFill, 5000);
      return;
    }
    new_content = evt.data;
    consLastIndex = 0;

  } else {
    if (consConn.readyState == 4) {
      message(errstr);
      setTimeout(consFill, 5000);
      return;
    }

    if (consConn.readyState != 3)
      return;

    var len = consConn.responseText.length;
    if (consLastIndex == len) // No new data
      return;

    new_content = consConn.responseText.substring(consLastIndex, len);
    consLastIndex = len;
  }
  if (new_content == undefined || new_content.length == 0)
    return;
  message("data received");
  console.log("Console Rcvd: " + new_content);

  consEvalContent(new_content);

}


function consFill() {
  message_clear();

  var query = "";
  var location = "http://node52.home.neubert-volmar.de:81";
  var loc = ("" + location).replace(/\?.*/, "");
  if (consConn) {
    consConn.close();
  }
  consConn = new WebSocket(loc.replace(/[&?].*/, '')
    .replace(/^http/i, "ws") + query);
  consConn.onclose =
    consConn.onerror =
    consConn.onmessage = consUpdate;

  consLastIndex = 0;
}


function consSetScrollFunction(n) {

  mustScroll[n]= 1;
  var element = $("#console" + n);
  element.scroll(function() { // autoscroll check

    if (element[0].scrollHeight - element.scrollTop() <=
      element.outerHeight() + 2) {
      if (!mustScroll[n]) {
        mustScroll[n] = 1;
        console.log("Console " + n + " autoscroll restarted");
      }
    } else {
      if (mustScroll[n]) {
        mustScroll[n] = 0;
        console.log("Console " + n + " autoscroll stopped");
      }
    }
  });
}


function consStart() {

  console.log("Console is opening");
  consSetScrollFunction(1);
  consSetScrollFunction(2);
  setTimeout(consFill, 1000);

}


window.onload = consStart;
