#include "FS.h"
#include "framework/Web/Dashboard.h"
#include <vector>

void Widget::toJsonObject(JsonObject &O) { O["name"] = name; }

void Widget::fromJsonObject(JsonObject &O) { name= O["name"].as<String>(); }

Dashboard::Dashboard(LOGGING& logging) : logging(logging) {}

Widget Dashboard::createWidget(JsonObject&) {
  // here we need to create descendants of widgets depending on O["type"]
  Widget w;
  return w;
}



bool Dashboard::load() {
  File file = SPIFFS.open(DASHBOARDFILENAME, "r");
  if(!file) {
    logging.error("could not open dashboard file "+String(DASHBOARDFILENAME));
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

void Dashboard::deserialize(JsonArray &A) {
  widgets.clear();
  logging.debug(String(A.size())+ " widgets in dashboard");
  for (JsonObject &O : A) {
    Widget w = createWidget(O);
    w.fromJsonObject(O);
    widgets.push_back(w);
  }
}

void Dashboard::serialize(String &JsonDocument) {
  DynamicJsonBuffer jsonBuffer(JSONBUFFERSIZE);
  JsonArray &A = jsonBuffer.createArray();
  for (Widget w : widgets) {
    JsonObject &O = A.createNestedObject();
    w.toJsonObject(O);
  }
  A.printTo(JsonDocument);
}

String Dashboard::asJsonDocument() {
  String jsonDocument;
  serialize(jsonDocument);
  return jsonDocument;
}
