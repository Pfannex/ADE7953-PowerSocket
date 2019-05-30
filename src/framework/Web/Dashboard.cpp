#include "framework/Web/Dashboard.h"
#include "FS.h"
#include "framework/Utils/Debug.h"
#include <vector>

//###############################################################################
//  Widget
//###############################################################################

Widget::Widget() { type = "<not set>"; }

Widget::Widget(String &t) { type = t; }

Widget::Widget(const char *t) { type = String(t); }

Widget::~Widget() {}

String Widget::getProperty(JsonObject &O, const char *property) {
  if (O.containsKey(property)) {
    String value = O[property].as<String>();
    // String msg = "Widget::getProperty()    " + String(property) + ": " +
    // value; D(msg.c_str());
    return value;
  } else {
    return "";
  }
}

void Widget::setProperty(JsonObject &O, const char *property, String value) {
  if (value.length() > 0) {
    O[property] = value;
    // String msg = "Widget::setProperty()    " + String(property) + ": " +
    // value; D(msg.c_str());
  }
}

void Widget::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  // D("Widget::toJsonObject");
  setProperty(O, "type", type);
  setProperty(O, "name", name);
}

void Widget::fromJsonObject(JsonObject &O) {
  type = getProperty(O, "type");
  name = getProperty(O, "name");
}

//###############################################################################
//  WidgetArray
//###############################################################################

WidgetArray::~WidgetArray() {
  for (auto it = widgets.begin(); it != widgets.end(); it++) {
    Widget *w = *it;
    delete w;
  }
  widgets.clear();
}

Widget *WidgetArray::createWidget(String &type) const {
  // here we need to create descendants of widgets depending on O["type"]
  if (type.equals("group")) {
    // D("creating WidgetGroup");
    return new WidgetGroup;
  } else if (type.equals("controlgroup")) {
    // D("creating WidgetControlGroup");
    return new WidgetControlGroup;
  } else if (type.equals("grid")) {
    // D("creating WidgetGrid");
    return new WidgetGrid;
  } else if (type.equals("button")) {
    // D("creating WidgetButton");
    return new WidgetButton;
  } else if (type.equals("text")) {
    // D("creating WidgetText");
    return new WidgetText;
  } else if (type.equals("checkbox")) {
    // D("creating WidgetCheckbox");
    return new WidgetCheckbox;
  } else if (type.equals("slider")) {
    // D("creating WidgetSlider");
    return new WidgetSlider;
  } else {
    SOFTWAREERROR;
  }
}

Widget *WidgetArray::createWidget(JsonObject &O) const {
  // String msg;
  // O.prettyPrintTo(msg);
  // msg= "type= "+O["type"].as<String>()+" name= "+O["name"].as<String>();
  // msg = "create widget from JsonObject: " + msg;
  // D(msg.c_str());
  String type = O["type"].as<String>();
  return createWidget(type);
}

JsonArray &WidgetArray::serialize(DynamicJsonBuffer &jsonBuffer) {
  // D("serializing widget array");
  JsonArray &A = jsonBuffer.createArray();
  for (auto w : widgets) {
    // String msg= "  widget type: "+w->type;
    // D(msg.c_str());
    JsonObject &O = A.createNestedObject(); // create object and add to array
    w->toJsonObject(jsonBuffer, O);
  }
  return A;
}

void WidgetArray::deserialize(JsonArray &A) {
  // D("deserializing widget array");
  widgets.clear();
  for (JsonObject &O : A) {
    //DF("");
    Widget *w = createWidget(O);
    // D("  widget created");
    w->fromJsonObject(O);
    widgets.push_back(w);
  }
  // D("deserializing done");
}

bool WidgetArray::removeWidget(String &name) {
  for (auto it = widgets.begin(); it != widgets.end();) {
    Widget *w = *it;
    if (w->name.equals(name)) {
      delete w;
      it = widgets.erase(it);
      return true;
    } else {
      if (w->type.equals("group")) {
        WidgetGroup *g = (WidgetGroup *)(w);
        if (g->removeWidget(name))
          return true;
      }
      ++it;
    }
  }
  return false;
}

Widget *WidgetArray::insertWidget(String &type, int position) {
  // D("insert widget");
  Widget *w = createWidget(type);
  int pos = position < 0 ? widgets.size() - position + 1 : position;
  auto it = widgets.begin();
  while (pos-- && it != widgets.end())
    it++;
  widgets.insert(it, w);
  return w;
}

Widget *WidgetArray::insertWidget(const char *type, int position) {
  String t = String(type);
  return insertWidget(t, position);
}

Widget *WidgetArray::insertWidget(String &type, String &group, int position) {
  for (auto it = widgets.begin(); it != widgets.end(); it++) {
    Widget *w = *it;
    if (w->type.equals("group")) {
      WidgetGroup *g = (WidgetGroup *)(w);
      Widget *n = g->insertWidget(type, group, position);
      if (n)
        return n;
    }
  }
  return nullptr;
}

Widget *WidgetArray::insertWidget(const char *type, const char *group,
                                  int position) {
  String t = String(type);
  String g = String(group);
  return insertWidget(t, g, position);
}

//###############################################################################
//  WidgetCaptioned
//###############################################################################

WidgetCaptioned::WidgetCaptioned(const char*) : Widget(type) {}

void WidgetCaptioned::toJsonObject(DynamicJsonBuffer &jsonBuffer,
                                   JsonObject &O) {
  Widget::toJsonObject(jsonBuffer, O);
  setProperty(O, "caption", caption);
}

void WidgetCaptioned::fromJsonObject(JsonObject &O) {
  Widget::fromJsonObject(O);
  caption = getProperty(O, "caption");
}

//###############################################################################
//  WidgetGrid
//###############################################################################

WidgetGrid::WidgetGrid() : Widget("grid") {}

void WidgetGrid::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  Widget::toJsonObject(jsonBuffer, O);
  JsonArray &A = jsonBuffer.createArray();
  for (WidgetArray *wa : data) {
    A.add(wa->serialize(jsonBuffer));
  }
  O["data"] = A;
}

void WidgetGrid::fromJsonObject(JsonObject &O) {
  Widget::fromJsonObject(O);
  data.clear();
  JsonArray &A = O["data"];
  for (JsonArray &WA : A) {
    WidgetArray *wa = new WidgetArray;
    wa->deserialize(WA);
    data.push_back(wa);
  }
}

WidgetArray *WidgetGrid::addRow() {
  WidgetArray *wa = new WidgetArray;
  data.push_back(wa);
  return wa;
}

//###############################################################################
//  WidgetGroup
//###############################################################################

WidgetGroup::WidgetGroup() : WidgetCaptioned("group") {}

WidgetGroup::~WidgetGroup() {}

void WidgetGroup::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  // D("WidgetGroup::toJsonObject");
  WidgetCaptioned::toJsonObject(jsonBuffer, O);
  setProperty(O, "caption", O["caption"]);
  O["data"] = data.serialize(jsonBuffer);
}

void WidgetGroup::fromJsonObject(JsonObject &O) {
  // D("WidgetGroup::fromJsonObject");
  WidgetCaptioned::fromJsonObject(O);
  caption = getProperty(O, "caption");
  data.deserialize(O["data"]);
}

Widget *WidgetGroup::insertWidget(String &type, String &group, int position) {
  if (name.equals(group)) {
    return data.insertWidget(type, position);
  } else {
    return data.insertWidget(type, group, position);
  }
}

bool WidgetGroup::removeWidget(String &name) { return data.removeWidget(name); }

//###############################################################################
//  WidgetAction
//###############################################################################

WidgetAction::WidgetAction(const char* type) : WidgetCaptioned(type) {};

void WidgetAction::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  WidgetCaptioned::toJsonObject(jsonBuffer, O);
  setProperty(O, "action", action);
}

void WidgetAction::fromJsonObject(JsonObject &O) {
  WidgetCaptioned::fromJsonObject(O);
  action = getProperty(O, "action");
}

//###############################################################################
//  WidgetButton
//###############################################################################

WidgetButton::WidgetButton() : WidgetAction("button"){};

void WidgetButton::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  WidgetAction::toJsonObject(jsonBuffer, O);
  setProperty(O, "icon", icon);
}

void WidgetButton::fromJsonObject(JsonObject &O) {
  WidgetAction::fromJsonObject(O);
  icon = getProperty(O, "icon");
}

//###############################################################################
//  WidgetInput
//###############################################################################

WidgetInput::WidgetInput(const char* type) : WidgetAction(type) {};

void WidgetInput::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  WidgetAction::toJsonObject(jsonBuffer, O);
  setProperty(O, "value", value);
  setProperty(O, "readonly", readonly);
  setProperty(O, "event", event);
}

void WidgetInput::fromJsonObject(JsonObject &O) {
  WidgetAction::fromJsonObject(O);
  value = getProperty(O, "value");
  readonly = getProperty(O, "readonly");
  event = getProperty(O, "event");
}

//###############################################################################
//  WidgetControlGroup
//###############################################################################

WidgetControlGroup::WidgetControlGroup() : WidgetInput("controlgroup") {};

void WidgetControlGroup::toJsonObject(DynamicJsonBuffer &jsonBuffer,
                                      JsonObject &O) {
  // D("WidgetControlGroup::toJsonObject");
  WidgetInput::toJsonObject(jsonBuffer, O);
  setProperty(O, "direction", direction);
  // D("serializing WidgetControlGroup elements");
  JsonArray &A = jsonBuffer.createArray();
  for (WidgetControlGroupElement E : data) {
    JsonObject &OE = A.createNestedObject(); // create object and add to array
    OE["label"] = E.label;
    OE["value"] = E.value;
  }
  O["data"] = A;
}

void WidgetControlGroup::fromJsonObject(JsonObject &O) {
  // D("WidgetControlGroup::fromJsonObject");
  WidgetInput::fromJsonObject(O);
  direction = getProperty(O, "direction");
  // D("deserializing WidgetControlGroup elements");
  JsonArray &A = O["data"];
  data.clear();
  for (JsonObject &OE : A) {
    WidgetControlGroupElement E;
    E.label = OE["label"].as<String>();
    E.value = OE["value"].as<String>();
    data.push_back(E);
  }
}

void WidgetControlGroup::appendElement(String &label, String &value) {
  WidgetControlGroupElement E;
  E.label = label;
  E.value = value;
  data.push_back(E);
}

void WidgetControlGroup::appendElement(const char *label, const char *value) {
  String l = String(label);
  String v = String(value);
  appendElement(l, v);
}



//###############################################################################
//  WidgetText
//###############################################################################

WidgetText::WidgetText() : WidgetInput("text") {};

void WidgetText::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  WidgetInput::toJsonObject(jsonBuffer, O);
  setProperty(O, "inputtype", inputtype);
  setProperty(O, "prefix", prefix);
  setProperty(O, "suffix", suffix);
}

void WidgetText::fromJsonObject(JsonObject &O) {
  WidgetInput::fromJsonObject(O);
  inputtype = getProperty(O, "inputtype");
  prefix = getProperty(O, "prefix");
  suffix = getProperty(O, "suffix");
}

//###############################################################################
//  WidgetCheckbox
//###############################################################################

WidgetCheckbox::WidgetCheckbox() : WidgetInput("checkbox") {};

//###############################################################################
//  WidgetSlider
//###############################################################################

WidgetSlider::WidgetSlider() : WidgetInput("slider") {};

void WidgetSlider::toJsonObject(DynamicJsonBuffer &jsonBuffer, JsonObject &O) {
  WidgetInput::toJsonObject(jsonBuffer, O);
  setProperty(O, "min", min);
  setProperty(O, "max", max);
}

void WidgetSlider::fromJsonObject(JsonObject &O) {
  WidgetInput::fromJsonObject(O);
  min = getProperty(O, "min");
  max = getProperty(O, "max");
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
