"use strict";

var withLog = 0;
var mustScroll = [1, 1];
var lines = [0, 0];
var timers = Object.create(null);

// ------------------------------------------------------------------------
// display consoles
// ------------------------------------------------------------------------

function consoleSet(n, msg) {
  $("#console" + n).html(msg);
}

function consoleScroll(n) {
  var h = $("#console" + n)[0].scrollHeight;
  //debugmsg("scrolling to " + h);
  $("#console" + n).scrollTop(h);
}

function consoleWriteln(n, msg) {
  var d = new Date();
  lines[n]++;
  if (lines[n] > 250) {
    var txt = $("#console" + n).text();
    $("#console" + n).text(txt.substring(txt.indexOf("\n") + 1));
  }
  $("#console" + n).append(d.toLocaleString() + " " + msg + "\n");
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


function consSetScrollFunction(n) {

  mustScroll[n] = 1;
  var element = $("#console" + n);
  element.scroll(function() { // autoscroll check

    if (element[0].scrollHeight - element.scrollTop() <=
      element.outerHeight() + 2) {
      if (!mustScroll[n]) {
        mustScroll[n] = 1;
        //debugmsg("Console " + n + " autoscroll restarted");
      }
    } else {
      if (mustScroll[n]) {
        mustScroll[n] = 0;
        //debugmsg("Console " + n + " autoscroll stopped");
      }
    }
  });
}


function consStart() {

  logmsg("Starting consoles...");
  consSetScrollFunction(0);
  consSetScrollFunction(1);

}


// ------------------------------------------------------------------------
// console logic
// ------------------------------------------------------------------------

function consAddReadings(nodeid) {

  //debugmsg("add readings grid to " + nodeid);
  // readings grid for topic topic1/topic2/topic3 is
  // identified by readings_topic1_topic2_r
  var gridid = nodeid + "_r";
  var content = "<div class='ui-grid-b' id='" + gridid + "'></div>";
  var child = $("#" + nodeid + " div.ui-collapsible-content").filter(":first").prepend(content);
  child.id = gridid;
  return child;
}

function consColorReading(rid, color) {
  $("#" + rid + "_t").css("color", color);
  $("#" + rid).css("color", color);
  $("#" + rid + "_v").css("color", color);
}

function consAddReading(gridid, rid, topic) {

  //debugmsg("add reading " + topic + " for " + rid + " to " + gridid);
  var content = "<div class='ui-block-a'><div class='ui-bar ui-bar-a' id='" + rid + "_t'>?</div></div>" +
    "<div class='ui-block-b'><div class='ui-bar ui-bar-a' id='" + rid + "'>" + topic + "</div></div>" +
    "<div class='ui-block-c'><div class='ui-bar ui-bar-a' id='" + rid + "_v'>?</div></div>";
  /*
  var content= "<div class='ui-block-a' id='"+rid+"_t'>?</div>"+
  "<div class='ui-block-b' id='"+rid+"'>"+topic+"</div>"+
  "<div class='ui-block-c' id='"+rid+"_v'>?</div>";*/
  var child = $("#" + gridid).append(content);
  child.id = rid;
  $("#" + gridid).trigger("refresh");
  return child;
}

function consSetReading(rid, time, value) {
  //debugmsg("set time " + time + " and value " + value + " for " + rid);
  $("#" + rid + "_t").text(time);
  value ? $("#" + rid + "_v").text(value) : $("#" + rid + "_v").html("&nbsp;");
  clearTimeout(timers[rid]);
  consColorReading(rid, "red");
  timers[rid] = setTimeout(function() {
    consColorReading(rid, "black");
  }, 2500);
}

function consAddNode(nodeid, rid, topic) {
  //debugmsg("add node for " + rid + " to " + nodeid + " for topic /" + topic + "/");
  var content = "<div data-role='collapsible' data-collapsed='false' id='" +
    rid + "'><h3>" + topic + "</h3></div>";
  var child = $("#" + nodeid + " div.ui-collapsible-content").filter(":first").append(content);
  child.id = rid;
  child.trigger('create');
  // instead of the previous line, the next lines makes an accordion
  // where only one collapsibleset is open at a time
  //child.collapsibleset().trigger('create');
  return child;
}

function consEvalEvent(time, topics, args) {
  var i;
  var rid = "readings";
  // do not modify time, topics, args here
  // skip device name and "event"
  for (i = 2; i < topics.length; i++) {
    var topic = topics[i];
    var parentid = rid;
    rid += "_" + topic;
    var element = document.getElementById(rid);
    if (element == null) {
      //debugmsg(rid + " not found");
      if (i < topics.length - 1) {
        // node
        consAddNode(parentid, rid, topic);
      } else {
        // rightmost topic = reading
        var gridid = parentid + "_r";
        if (document.getElementById(gridid) == null) {
          // add a readings grid to the parent node
          consAddReadings(parentid);
        }
        consAddReading(gridid, rid, topic);
      }
    }
    if (i == topics.length - 1) {
      consSetReading(rid, time, args);
    }
  }
}
