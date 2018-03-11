"use strict";

var w1 = {
  "type": "controlgroup",
  "name": "power",
  "legend": "Power",
  "action": "~/set/device/power",
  "event": "~/event/device/power",
  "data": [{
      "label": "Off",
      "value": 0
    },
    {
      "label": "On",
      "value": 1
    },
  ]
}

var w0 = {
  "type": "text",
  "name": "clock",
  "legend": "Time",
  "readonly": 1,
  "event": "~/event/clock/time",
  "inputtype": "datetime"
}

var w2 = {
  "type": "text",
  "name": "brightness",
  "legend": "brightness",
  "action": "~/set/device/brightness",
  "event": "~/event/device/brightness",
  "inputtype": "number"
}



var widgets = [w0, w1, w2];



// ---------------------------
// dashboard
// ---------------------------

var muteDashboardActions = 0;



//
// widgets
//

function dashboardAddContent(w, content) {
  debugmsg(content);
  var child = $("#dashboard div.ui-collapsible-content").append(content);
  child.trigger('create');
}

// http://api.jquerymobile.com/controlgroup/
function dashboardAddControlgroup(w) {

  var legend = w.legend;
  var name = w.name;
  var data = w.data;

  var content = "<!-- widget: controlgroup " + name + " -->\n" +
    "<div class=\"ui-field-contain\">\n" +
    "<fieldset data-role=\"controlgroup\" data-type=\"horizontal\" data-mini=\"true\" " +
    "onChange=\"dashboardAction('" + name + "')\">\n" +
    "<legend>" + legend + "</legend>\n";
  var i;
  for (i = 0; i < data.length; i++) {
    var label = data[i].label;
    var value = data[i].value;
    var id = name + "-" + value;
    logmsg(" with label " + label + ", value " + value);
    content += "<input name=\"" + name + "\" id=\"" + id + "\" value=\"" + value + "\" type=\"radio\"/>\n";
    content += "<label for=\"" + id + "\">" + label + "</label>\n";
  }
  content += "</fieldset>\n</div>\n";
  dashboardAddContent(w, content);
}

// http://api.jquerymobile.com/textinput/
function dashboardAddText(w) {

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
  dashboardAddContent(w, content);
}


//
// dashboard builder
//

function dashboardBuild() {

  logmsg("Building dashboard...");

  // widget= JSON.parse(text)

  var i;
  for (i = 0; i < widgets.length; i++) {
    var w = widgets[i];
    switch (w.type) {
      case "controlgroup":
        dashboardAddControlgroup(w);
        ""
        break;
      case "text":
        dashboardAddText(w);
        break;
      default:
        logmsg("Unknown widget type " + w.type);
    }
  }
  logmsg("Dashboard ready.");
}


//
// dashboard action
//
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
