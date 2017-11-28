"use strict";

var consConn;

var consLastIndex = 0;
var withLog = 0;
var mustScroll= [1, 1];
var lines= [0, 0];
var timers = Object.create(null);

// ------------------------------------------------------------------------
// display consoles
// ------------------------------------------------------------------------

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
  lines[n]++;
  if(lines[n]> 250) {
    var txt= $("#console" + n).text();
    $("#console" + n).text(txt.substring(txt.indexOf("\n")+1));
  }
  $("#console" + n).append(d.toLocaleString()+" "+msg + "\n");
  if (mustScroll[n]) {
    consoleScroll(n);
  }
}

function consoleClear(n) {
  consoleSet(n, "");
}

// ------------------------------------------------------------------------
// console handling
// ------------------------------------------------------------------------


function consAddReadings(nodeid) {

  console.log("add readings grid to "+nodeid);
  // readings grid for topic topic1/topic2/topic3 is
  // identified by readings_topic1_topic2_r
  var gridid= nodeid+"_r";
  var content= "<div class='ui-grid-b' id='"+gridid+"'></div>";
  var child= $("#"+nodeid+" div.ui-collapsible-content").filter(":first").prepend(content);
  child.id= gridid;
  return child;
}

function consColorReading(rid, color) {
  $("#"+rid+"_t").css("color", color);
  $("#"+rid).css("color", color);
  $("#"+rid+"_v").css("color", color);
}

function consAddReading(gridid, rid, topic) {

  console.log("add reading "+topic+" for "+rid+" to "+gridid);
  var content= "<div class='ui-block-a'><div class='ui-bar ui-bar-a' id='"+rid+"_t'>?</div></div>"+
  "<div class='ui-block-b'><div class='ui-bar ui-bar-a' id='"+rid+"'>"+topic+"</div></div>"+
  "<div class='ui-block-c'><div class='ui-bar ui-bar-a' id='"+rid+"_v'>?</div></div>";
  /*
  var content= "<div class='ui-block-a' id='"+rid+"_t'>?</div>"+
  "<div class='ui-block-b' id='"+rid+"'>"+topic+"</div>"+
  "<div class='ui-block-c' id='"+rid+"_v'>?</div>";*/
  var child= $("#"+gridid).append(content);
  child.id= rid;
  $("#"+gridid).trigger("refresh");
  return child;
}

function consSetReading(rid, time, value) {
  //console.log("set time "+time+" and value "+value+" for "+rid);
  $("#"+rid+"_t").text(time);
  value ? $("#"+rid+"_v").text(value) :  $("#"+rid+"_v").html("&nbsp;");
  clearTimeout(timers[rid]);
  consColorReading(rid, "red");
  timers[rid]= setTimeout( function() { consColorReading(rid, "black"); }, 2500);
}

function consAddNode(nodeid, rid, topic) {
  console.log("add node for "+rid+" to "+nodeid+" for topic /"+topic+"/");
  var content = "<div data-role='collapsible' data-collapsed='false' id='"+
    rid+"'><h3>"+topic+"</h3></div>";
  var child= $("#"+nodeid+" div.ui-collapsible-content").filter(":first").append(content);
  child.id= rid;
  child.trigger('create');
  // instead of the previous line, the next lines makes an accordion
  // where only one collapsibleset is open at a time
  //child.collapsibleset().trigger('create');
  return child;
}

function consEvalEvent(time, content) {
  var topicsArgs= content.split(" ");
  var topics= topicsArgs[0].split("/");
  topics.shift(); topics.shift(); // remove device name and "event"
  topicsArgs.shift();
  var args= topicsArgs.length > 0 ? topicsArgs.join(" ") : "";
  var i;
  var rid= "readings";
  for(i= 0; i< topics.length; i++) {
    var topic= topics[i];
    var parentid= rid;
    rid+= "_"+topic;
    var element= document.getElementById(rid);
    if(element == null) {
      console.log(rid+" not found!");
      if(i< topics.length-1) {
        // node
        consAddNode(parentid, rid, topic);
      } else {
        // rightmost topic = reading
        var gridid= parentid+"_r";
        if(document.getElementById(gridid) == null) {
          // add a readings grid to the parent node
          consAddReadings(parentid);
        }
        consAddReading(gridid, rid, topic);
      }
    }
    if(i== topics.length-1) {
      consSetReading(rid, time, args);
    }
  }
}

function consEvalContent(content) {
  var fields= JSON.parse(content);
  if(fields.type=="event") {
    consoleWriteln(0, fields.value);
    var d = new Date();
    consEvalEvent(d.toLocaleString(), fields.value);
  } else if (fields.type=="log") {
    consoleWriteln(1, fields.subtype+" "+fields.value)
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
  //console.log("Console received: " + new_content);

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
  var url= loc.replace(/[&?].*/, '').replace(/^http/i, "ws") + query;
  consConn = new WebSocket(url) /*, [], { headers: { "token": "foobar"}});*/
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
