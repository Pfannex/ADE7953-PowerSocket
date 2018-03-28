"use strict";

// ---------------------------
// dashboard
// ---------------------------

var muteDashboardActions = 0;
var widgets; // flat list of all widgets

//
// widgets
//

function dashboardAddContent(element, w, content) {
  debugmsg(content);
  widgets.push(w);
  var child = element.append(content);
  child.trigger('create');
  return child;
}

function dashboardAddGroup(element, w) {

  var name = w.name;
  var id = name;
  var legend = w.legend;
  var data = w.data; // array of elements

  var content = "<!-- widget: group " + name +"-->\n"+
    "<div data-role='collapsible' data-inset='true' data-collapsed='false' id='"+id+"'>"+
    '<h2>'+legend+'</h2>\n'+
    '</div>\n';
  //var child= dashboardAddContent(element, w, content).find("div.ui-collapsible-content");
  dashboardAddContent(element, w, content);
  var child= $("#"+id+" div.ui-collapsible-content");
  dashboardBuildArray(child, data);
}

// http://api.jquerymobile.com/controlgroup/
function dashboardAddControlgroup(element, w) {

  var legend = w.legend;
  var name = w.name;
  var data = w.data;
  var direction = w.direction
  if (typeof w.direction === "undefined") {
    direction= "horizontal";
  } else {
    direction= w.direction;
  }

  var content = "<!-- widget: controlgroup " + name + " -->\n" +
    "<div class=\"ui-field-contain\">\n" +
    "<fieldset data-role=\"controlgroup\" data-type=\""+direction+"\" data-mini=\"true\" " +
    "onChange=\"dashboardAction('" + name + "')\">\n" +
    "<legend>" + legend + "</legend>\n";
  var i;
  for (i = 0; i < data.length; i++) {
    var label = data[i].label;
    var value = data[i].value;
    var id = name + "-" + value;
    //debugmsg(" with label " + label + ", value " + value);
    content += "<input name=\"" + name + "\" id=\"" + id + "\" value=\"" + value + "\" type=\"radio\"/>\n";
    content += "<label for=\"" + id + "\">" + label + "</label>\n";
  }
  content += "</fieldset>\n</div>\n";
  dashboardAddContent(element, w, content);
}

// http://api.jquerymobile.com/textinput/
function dashboardAddText(element, w) {

  var legend = w.legend;
  var name = w.name;
  var data = w.data;
  var inputtype = w.inputtype;
  var readonly = w.readonly;
  var id = name;

  var content = "<!-- widget: text " + name + " -->\n" +
    "<div class=\"ui-field-contain\">\n" +
    "<label for=\"" + id + "\">" + legend + "</label>\n" +
    "<input type=\"" + inputtype + "\" data-mini=\"true\" " +
    "id=\"" + id + "\" value=\"\" " +
    (readonly ? "readonly " : "") +
    "onChange=\"dashboardAction('" + name + "')\">\n" +
    "</div>\n";
  dashboardAddContent(element, w, content);
}

// http://api.jquerymobile.com/button/
function dashboardAddButton(element, w) {

  var legend = w.legend;
  var name = w.name;
  var id = name;

  var content = "<!-- widget: button " + name + " -->\n" +
    "<button class=\"ui-btn ui-btn-inline\" "+
    "onClick=\"dashboardAction('" + name + "')\">"+legend+"</button> \n";
  dashboardAddContent(element, w, content);
}

// http://api.jquerymobile.com/slider/
function dashboardAddSlider(element, w) {

  var legend = w.legend;
  var name = w.name;
  var id = name;
  var min= w.min; if (typeof min === "undefined") min= 0;
  var max= w.max; if (typeof max === "undefined") max= 99;

  var content = "<!-- widget: slider " + name + " -->\n" +
    "<div class=\"ui-field-contain\">\n" +
    "<label for=\"" + id + "\">" + legend + "</label>\n" +
    "<input type=\"range\" id=\"" + id + "\" " +
    "min=\""+min+"\" max=\""+max+"\" value=\""+min+"\" "+
    "onInput=\"dashboardAction('" + name + "')\" "+
    "onChange=\"dashboardAction('" + name + "')\">\n" +
    "</div>\n";
  dashboardAddContent(element, w, content);
}

//
// dashboard builder
//

//
// retrieve configuration from device
//
function retrieveDashboard(callback) {
  logmsg("Retrieving dashboard from device...");
  return call("~/get/ffs/webCFG/root", callback);
}

function dashboardBuildArray(element, widgets) {
  var i;
  for (i = 0; i < widgets.length; i++) {
    var w = widgets[i];
    switch (w.type) {
      case "group":
        dashboardAddGroup(element, w);
        break;
      case "controlgroup":
        dashboardAddControlgroup(element, w);
        break;
      case "text":
        dashboardAddText(element, w);
        break;
      case "button":
        dashboardAddButton(element, w);
        break;
      case "slider":
        dashboardAddSlider(element, w);
        break;
      default:
        logmsg("Unknown widget type " + w.type);
    }
  }
}

function dashboardBuild(json) {

  logmsg("Building dashboard...");
  debugmsg(json);
  widgets= []; // clear widget list
  var element= $("#dashboard");
  dashboardBuildArray(element, JSON.parse(json));
  logmsg("Dashboard ready.");
}


//
// dashboard action
//
var MIN_MSECS_BETWEEN_CALLS= 200;
var lastCallTime= 0;
var lastTopicsArgs= "";
var delayActive= 0;

function doCall() {
  call(lastTopicsArgs);
  lastCallTime= (new Date()).getTime();
  delayActive= 0;
}

function limitCall(topicsArgs) {
    // ensure that the API is not called more often the every 300 ms
    lastTopicsArgs= topicsArgs;
    if(delayActive) return;
    var t= (new Date()).getTime();
    var dt= t-lastCallTime;
    if(dt< MIN_MSECS_BETWEEN_CALLS) {
      // try again later
      delayActive= 1;
      setTimeout(doCall, MIN_MSECS_BETWEEN_CALLS-dt);
      debugmsg("API call delayed by "+dt+" ms.");
    } else {
      debugmsg("Immediate API call.");
      doCall();
    }
}

function dashboardAction(name) {

  // no action should occur if an event changes the dashboard
  if (muteDashboardActions) return;

  // look for a matching widget
  var i;
  for (i = 0; i < widgets.length; i++) {
    if (widgets[i].name == name) {
      var w = widgets[i];
      if (w.action != "") {
        switch (w.type) {
          case "controlgroup":
            var value = getRadio(w.name);
            if(w.action) {
              call(w.action + " " + value);
            }
            logmsg("controlgroup " + name + " has changed to " + value);
            break;
          case "text":
            var value = getText(w.name);
            logmsg("text " + name + " has changed to " + value);
            if(w.action) {
              call(w.action + " " + value);
            }
            break;
          case "button":
            logmsg("button "+ name + " clicked");
            if(w.action) {
              limitCall(w.action);
            }
            break;
          case "slider":
            var value = getText(w.name);
            logmsg("slider " + name + " has changed to " + value);
            if(w.action) {
              limitCall(w.action + " " + value);
            }
            break;
          default:
            logmsg("Unknown widget type " + w.type);
        }
      }
      break;
    }
  }
}

//
// dashboardEvalEvent
//
function dashboardEvalEvent(topics, args) {

  muteDashboardActions = 1;
  var topicStr = topics.join("/");
  //debugmsg("dashboard event for topic " + topicStr + " with arg " + args);

  // look for a matching widget
  var i;
  for (i = 0; i < widgets.length; i++) {
    if (widgets[i].event == topicStr) {
      var w = widgets[i];
      switch (w.type) {
        case "controlgroup":
          setRadio(w.name, args);
          break;
        case "text":
          setText(w.name, args);
          break;
        case "slider":
          setText(w.name, args);
          break;
        default:
          logmsg("Unknown widget type " + w.type);
      }
      break;
    }
  }

  muteDashboardActions = 0;

}
// ------------------------------------------------------------------------
// dashboard handling
// ------------------------------------------------------------------------


// unused

function handleDashboardClick(widget) {
  //debugmsg("click for "+widget.name);
  if (widget.type == "checkbox") debugmsg("checkbox " + widget.checked);
  else
  if (widget.type == "number") debugmsg("number " + widget.value);
  if (widget.checkValidity()) debugmsg("Valid");
  else debugmsg("INVALID!");
}
