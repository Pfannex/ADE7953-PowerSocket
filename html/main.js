"use strict";

// ---------------------------
// initialization
// ---------------------------

$(document).ready(function() {
  logmsg("Document loaded.");
});

// initialize the dashboard
$(document).on("pagecreate", "#page1", function(event, ui) {
  // moved here, otherwise executed for any page that uses omniesp.js
  logmsg("Page 1 created.");
  consStart();
  startListener();
  logmsg('Initializing dashboard...');
  var json = retrieveDashboard(dashboardBuild);
})

// initialize the config page
$(document).on("pagecreate", "#page2", function(event, ui) {
  logmsg("Page 2 created.");
  logmsg('Initializing configuration page...');
  var json = retrieveConfig(setConfig);
})

// ---------------------------
// go back to the login page
// ---------------------------

function startOver() {
  closeConnection();
  window.location.href = "/";
}

// ---------------------------
// status bar
// ---------------------------

function message(msg) {
  //$("#message").html(msg);
  window.status = msg;
  debugmsg(msg);
}

function message_clear() {
  message("");
}

// ---------------------------
// websocket listener
// ---------------------------

var connection;

function startListener() {

  logmsg("Opening connection...");
  setTimeout(openConnection, 1000);

}

function openConnection() {
  message_clear();

  var location = window.location.href; //"http://node52.home.neubert-volmar.de:80/goo.html";
  var parts = location.split("/");
  var url = 'ws://' + parts[2] + '/ws';
  //url = 'ws://node52.home.neubert-volmar.de/ws';
  if (connection) {
    connection.close();
  }
  logmsg("Connecting to " + url + "...");
  connection = new WebSocket(url) /*, [], { headers: { "token": "foobar"}});*/
  connection.onopen= onOpen;
  connection.onclose = onClose;
  connection.onerror = onError;
  connection.onmessage = onMessage;

}

function onOpen() {
  logmsg("Websocket connection established.");
}

function onMessage(evt) {
  debugmsg("Message received: '"+evt.data+"'");
  var msgs= evt.data.split("\r\n");
  msgs.forEach(dispatchContent);
}

function onError() {
  message("Connection lost, trying to reconnect every 5 seconds.");
  closeConnection();
  setTimeout(openConnection, 5000);
}

function onClose() {
  logmsg("Websocket connection closed.");  
}

function closeConnection() {
  if (!connection)
    return;
  if (typeof connection.close == "function")
    connection.close();
  else if (typeof connection.abort == "function")
    connection.abort();
  connection = undefined;
}

// ---------------------------
// dispatch content
// ---------------------------

function dispatchContent(content) {
  var fields = JSON.parse(content);
  //debugmsg("content type "+fields.type);
  if (fields.type == "event") {
    // events are written to console 0
    consoleWriteln(0, fields.value);
    // further processing of events...
    // date
    var d = new Date();
    // split content into topics and args
    var topicsArgs = fields.value.split(" ");
    // topics
    var topics = topicsArgs[0].split("/");
    topics.shift(); // remove device name
    topics.unshift("~"); // normalize device name
    // args
    topicsArgs.shift();
    var args = topicsArgs.length > 0 ? topicsArgs.join(" ") : "";
    // in the console (readings)
    consEvalEvent(d.toLocaleString(), topics, args);
    // in the dashboard
    dashboardEvalEvent(topics, args);
    // in the UI
    uiEvalEvent(topics, args);
  } else if (fields.type == "log") {
    // log messages are written to console 1
    consoleWriteln(1, fields.subtype + " " + fields.value)
  }
}
