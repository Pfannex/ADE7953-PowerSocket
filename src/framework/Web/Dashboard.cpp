#include "FS.h"
#include "framework/Utils/Debug.h"
#include "framework/Web/Dashboard.h"
#include <vector>

//###############################################################################
//  Widget
//###############################################################################

String Widget::getProperty(JsonObject &O, const char *property) {
  if (O.containsKey(property)) {
    String value = O[property].as<String>();
    String msg = "Widget::getProperty()    " + String(property) + ": " + value;
    D(msg.c_str());
    return value;
  } else {
    return "";
  }
}

void Widget::setProperty(JsonObject &O, const char *property, String value) {
  if (value.length() > 0) {
    O[property] = value;
    String msg = "Widget::setProperty()    " + String(property) + ": " + value;
    D(msg.c_str());
  }
}

void Widget::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  D("Widget::toJsonObject");
  setProperty(O, "type", type);
  setProperty(O, "name", name);
  setProperty(O, "caption", caption);
  setProperty(O, "action", action);
  setProperty(O, "event", event);
  setProperty(O, "inputtype", inputtype);
  setProperty(O, "readonly", readonly);
  setProperty(O, "min", min);
  setProperty(O, "max", max);
}

void Widget::fromJsonObject(JsonObject &O) {
  type = getProperty(O, "type");
  name = getProperty(O, "name");
  caption = getProperty(O, "caption");
  action = getProperty(O, "action");
  event = getProperty(O, "event");
  inputtype = getProperty(O, "inputtype");
  readonly = getProperty(O, "readonly");
  min = getProperty(O, "min");
  max = getProperty(O, "max");
}

//###############################################################################
//  WidgetArray
//###############################################################################

Widget* WidgetArray::createWidget(JsonObject &O) const {
  // here we need to create descendants of widgets depending on O["type"]
  String type = O["type"];
  if (type.equals("group")) {
    D("creating WidgetGroup");
    return new WidgetGroup;
  } else {
    D("creating Widget");
    return new Widget;
  }
}

JsonArray &WidgetArray::serialize(DynamicJsonBuffer &jsonBuffer) {
  D("serializing widget array");
  JsonArray &A = jsonBuffer.createArray();
  for (auto w : widgets) {
    String msg= "  widget type: "+w->type;
    D(msg.c_str());
    JsonObject &O = A.createNestedObject(); // create object and add to array
    w->toJsonObject(jsonBuffer, O);
  }
  return A;
}

void WidgetArray::deserialize(JsonArray &A) {
  D("deserializing widget array");
  widgets.clear();
  for (JsonObject &O : A) {
    D("  widget");
    Widget* w = createWidget(O);
    w->fromJsonObject(O);
    widgets.push_back(w);
  }
}

//###############################################################################
//  WidgetGroup
//###############################################################################

void WidgetGroup::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  D("WidgetGroup::toJsonObject");
  Widget::toJsonObject(jsonBuffer, O);
  D(">");
  O["data"]= data.serialize(jsonBuffer);
  D("<");

}

void WidgetGroup::fromJsonObject(JsonObject &O) {
  Widget::fromJsonObject(O);
  D(">");
  data.deserialize(O["data"]);
  D("<");
}

//###############################################################################
//  Dashboard
//###############################################################################

Dashboard::Dashboard(LOGGING &logging) : logging(logging) {}

bool Dashboard::load() {
  File file = SPIFFS.open(DASHBOARDFILENAME, "r");
  if (!file) {
    logging.error("could not open dashboard file " + String(DASHBOARDFILENAME));
    return false;
  }
  DynamicJsonBuffer jsonBuffer(JSONBUFFERSIZE);
  JsonArray &A = jsonBuffer.parseArray(file);
  file.close();
  if (A.success()) {
    logging.info("dashboard successfully parsed");
    deserialize(A);
    return true;
  } else {
    logging.error("dashboard parsing failed");
    return false;
  }
}

String Dashboard::asJsonDocument() {
  DynamicJsonBuffer jsonBuffer(JSONBUFFERSIZE);
  JsonArray &A = serialize(jsonBuffer);
  String jsonDocument;
  A.printTo(jsonDocument);
  return jsonDocument;
}
