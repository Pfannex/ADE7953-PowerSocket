"use strict";

// ---------------------------
// dashboard
// ---------------------------

//
// apiCall
//

function apiCall(value) {
  $('textarea#apiresult').val("");
  call(value, showApiCallResult);
}

function showApiCallResult(data) {
  $('textarea#apiresult').val(data);
}

var muteDashboardActions = 0;
var widgets; // flat list of all widgets

//
// retrieve configuration from device
//
function retrieveDashboard(callback) {
  logmsg("Retrieving dashboard from device...");
  //return call("~/get/ffs/webCFG/root", callback);
  return call("~/get/device/dashboard", callback);
}

//
// widgets
//

function dashboardGetWidget(w) {
  widgets.push(w);
  switch (w.type) {
    case "group":
      return dashboardGetGroup(w);
    case "grid":
      return dashboardGetGrid(w);
      break;
    case "controlgroup":
      return dashboardGetControlgroup(w);
    case "text":
      return dashboardGetText(w);
    case "button":
      return dashboardGetButton(w);
    case "checkbox":
      return dashboardGetCheckbox(w);
    case "slider":
      return dashboardGetSlider(w);
    default:
      logmsg("Unknown widget type " + w.type);
      return "";
  }
}

function dashboardGetWidgets(a) {
  var content = "";
  var i;
  for (i = 0; i < a.length; i++) {
    var w = a[i];
    content += dashboardGetWidget(w);
  }
  return content;
}

function dashboardGetGroup(w) {

  var name = w.name;
  var id = name;
  var caption = w.caption;
  var data = w.data; // array of elements
  var collapsed = w.collapsed ? w.collapsed : "false";

  var content = '<!-- widget: group ' + name + '-->\n' +
    '<div data-role="collapsible" data-inset="true" data-collapsed="'+
    collapsed+'" id="' + id + '">' +
    '<h2>' + caption + '</h2>\n' +
    dashboardGetWidgets(data) +
    '</div>\n';
  return content;
}

// http://demos.jquerymobile.com/1.4.5/grids/
function dashboardGetGrid(w) {

  var name = w.name;
  var data = w.data; // array of array elements
  var rows = data.length;
  var columns = rows ? data[0].length : 0;
  var row, column;
  var cellId;

  // build the grid
  var content = '<!-- widget: grid ' + name + '-->\n' +
    '<div class="no-margin-grid">'+
    '<fieldset class="ui-grid-' + String.fromCharCode(95 + columns) + '">\n';
  for (row = 0; row < rows; row++) {
    for (column = 0; column < data[row].length; column++) {
      cellId = name + '-' + row + '-' + column;
      content += '<div class="ui-block-' + String.fromCharCode(97 + column) +
        '" id="' + cellId + '">\n' +
        dashboardGetWidget(data[row][column]) +
        '</div>\n';
    }
    content += '\n';
  }
  content += '</fieldset></div>\n';
  return content;
}

// http://api.jquerymobile.com/controlgroup/
function dashboardGetControlgroup(w) {

  var caption = w.caption;
  var name = w.name;
  var data = w.data;
  var direction = w.direction
  if (typeof w.direction === "undefined") {
    direction = "horizontal";
  } else {
    direction = w.direction;
  }

  var content = '<!-- widget: controlgroup ' + name + ' -->\n' +
    '<div class="ui-field-contain">\n' +
    '<fieldset data-role="controlgroup" data-type="' + direction + '" data-mini="true" ' +
    'onChange="dashboardAction(\'' + name + '\')\">\n' +
    '<legend>' + caption + '</legend>\n';
  var i;
  for (i = 0; i < data.length; i++) {
    var label = data[i].label;
    var value = data[i].value;
    var id = name + "-" + value;
    //debugmsg(" with label " + label + ", value " + value);
    content += '<input name="' + name + '" id="' + id + '" value="' + value + '" type="radio"/>\n';
    content += '<label for="' + id + '">' + label + '</label>\n';
  }
  content += '</fieldset>\n</div>\n';
  return content;
}

// http://api.jquerymobile.com/textinput/
function dashboardGetText(w) {

  var caption = w.caption;
  // create a hidden caption to align texts in a grid
  // the additional blank ensures grid spacing
  //if(!w.caption) { caption= " "; } else { caption= " "+w.caption;}
  var name = w.name;
  var value = w.value;
  if(!value) {
    value = "";
  }
  var inputtype = w.inputtype;
  if (!inputtype) {
    inputtype = "text";
  }
  var readonly = w.readonly;
  var id = name;

  var content = '<!-- widget: text ' + name + ' -->\n' +
    '<div class="ui-field-contain">\n';
  if (caption) {
    content += '<label for="' + id + '">' + caption + '</label>\n';
  }
  content +=
    '<input type="' + inputtype + '" data-mini="true" ' +
    'id="' + id + '" value="'+value+'" ' +
    (readonly ? 'readonly ' : '');
  if (!readonly) {
    content += 'onChange="dashboardAction(\'' + name + '\')"';
  }
  content += '>\n' +
    '</div>\n';
  return content;
}

// http://api.jquerymobile.com/button/
// http://demos.jquerymobile.com/1.4.5/icons/
function dashboardGetButton(w) {

  var caption = w.caption;
  if(!caption) caption= "";
  var name = w.name;
  var id = name;
  var icon = w.icon;


  var uiicon= icon ?
    (" ui-icon-" + icon + " ui-btn-icon-" +
      ( caption ? "left" :  "notext")
    ) : "";

  var content = '<!-- widget: button ' + name + ' -->\n' +
    '<button class="ui-btn ui-btn-inline' + uiicon+'" ' +
    'onClick="dashboardAction(\'' + name + '\')">' + caption + '</button>\n';
  return content;
}

// http://api.jquerymobile.com/checkbox/
function dashboardGetCheckbox(w) {

  var caption = w.caption;
  var name = w.name;
  var readonly = w.readonly;
  var id = name;

  var content = '<!-- widget: checkbox ' + name + ' -->\n' +
    '<input type="checkbox" name="' + name + '" id="' + id + '" ' +
    (readonly ? 'readonly disabled ' : '');
    if (!readonly) {
      content += 'onChange="dashboardAction(\'' + name + '\')"';
    }
    content += '>\n' +
    '<label for="' + id + '">' + caption + '</label>\n';
  return content;
}

// http://api.jquerymobile.com/slider/
function dashboardGetSlider(w) {

  var caption = w.caption;
  var name = w.name;
  var id = name;
  var min = w.min;
  if (typeof min === "undefined") min = 0;
  var max = w.max;
  if (typeof max === "undefined") max = 99;

  var content = '<!-- widget: slider ' + name + ' -->\n' +
    '<div class="ui-field-contain">\n' +
    '<label for="' + id + '">' + caption + '</label>\n' +
    '<input type="range" id="' + id + '" ' +
    'min="' + min + '" max="' + max + '" value="' + min + '" ' +
    'onInput="dashboardAction(\'' + name + '\')" ' +
    'onChange="dashboardAction(\'' + name + '\')">\n' +
    '</div>\n';
  return content;
}

//
// dashboard builder
//

function dashboardBuild(json) {

  logmsg("Building dashboard...");
  debugmsg("json building plan follows...");
  debugmsg(json);
  widgets = []; // clear widget list
  var content = dashboardGetWidgets(JSON.parse(json));
  debugmsg("HTML code follows...");
  debugmsg(content);
  var element = $("#dashboard");
  var child = element.html(content);
  child.trigger('create');
  setTimeout( function() { call("~/set/device/fillDashboard"); }, 2000);
  logmsg("Dashboard ready.");
}

// -------------------------------
// dashboard action
// -------------------------------

var MIN_MSECS_BETWEEN_CALLS = 200;
var lastCallTime = 0;
var lastTopicsArgs = "";
var delayActive = 0;

function doCall() {
  call(lastTopicsArgs);
  lastCallTime = (new Date()).getTime();
  delayActive = 0;
}

function limitCall(topicsArgs) {
  // ensure that the API is not called more often the every 300 ms
  lastTopicsArgs = topicsArgs;
  if (delayActive) return;
  var t = (new Date()).getTime();
  var dt = t - lastCallTime;
  if (dt < MIN_MSECS_BETWEEN_CALLS) {
    // try again later
    delayActive = 1;
    setTimeout(doCall, MIN_MSECS_BETWEEN_CALLS - dt);
    debugmsg("API call delayed by " + dt + " ms.");
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
            if (w.action) {
              call(w.action + " " + value);
            }
            logmsg("controlgroup " + name + " has changed to " + value);
            break;
          case "text":
            var value = getText(w.name);
            logmsg("text " + name + " has changed to " + value);
            if (w.action) {
              call(w.action + " " + value);
            }
            break;
          case "button":
            logmsg("button " + name + " clicked");
            if (w.action) {
              limitCall(w.action);
            }
            break;
          case "checkbox":
            var value = getCheckbox(w.name) ? 1 : 0;
            logmsg("checkbox " + name + " has changed to " + value);
            if (w.action) {
              limitCall(w.action + " " + value);
            }
            break;
          case "slider":
            var value = getText(w.name);
            logmsg("slider " + name + " has changed to " + value);
            if (w.action) {
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

  // rebuild dashboard on event
  if(topicStr == "~/event/device/dashboardChanged") {
    logmsg("Requested dashboard rebuild");
    var json = retrieveDashboard(dashboardBuild);
    return;
  }

  // look for a matching widget
  var i;
  for (i = 0; i < widgets.length; i++) {
    if (widgets[i].event == topicStr) {
      var w = widgets[i];
      //debugmsg("event matches widget "+w.name);
      switch (w.type) {
        case "controlgroup":
          setRadio(w.name, args);
          break;
        case "text":
          var value = "";
          if (w.prefix) {
            value = w.prefix;
          }
          value += args;
          if (w.suffix) {
            value+= w.suffix;
          }
          setText(w.name, value);
          break;
        case "slider":
          setText(w.name, args);
          break;
        case "checkbox":
          setCheckbox(w.name, args);
          break;
        default:
          logmsg("Unknown widget type " + w.type);
      }
      //break; // do not stop here, event could match more than one widget
    }
  }

  muteDashboardActions = 0;

}
