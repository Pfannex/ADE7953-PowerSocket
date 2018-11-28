#include "framework/Utils/Logger.h"
#include <Arduino.h>
#include <ArduinoJson.h>

// https://arduinojson.org/v5/doc/

#define JSONBUFFERSIZE 4096
#define DASHBOARDFILENAME "/customDevice/dashboard.json"

class Widget {

private:
  String getProperty(JsonObject &O, const char *property);
  void setProperty(JsonObject &O, const char *property, String value);

public:
  String name; // must be unique across the whole structure
  String type;
  String caption;
  String action;
  String event;
  String inputtype;
  String readonly;
  String min;
  String max;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
};

class WidgetArray {
public:
  std::vector<Widget*> widgets;
  virtual JsonArray &serialize(DynamicJsonBuffer &);
  virtual void deserialize(JsonArray &);
  /*
  // removes the named widget from anywhere in the structure
  bool removeWidget(String name);
  // add the widget to the named group at given position
  // position is counted from the end if negative
  Widget* insertWidget(String type, String group, int position= -1);

  usage:
  // append a group named "roster" to dashboard
  WidgetGroup* g= dashboard.insertWidget("group");
  g->name= "roster";
  // append a text widget to the group "roster"
  Widget* w= dashboard.insertWidget("text", "roster");

  */
private:
  // http://www.dorodnic.com/blog/2014/03/29/abstract-factory/
  Widget* createWidget(JsonObject &) const; // abstract factory
};

class WidgetGroup : public Widget {
public:
  WidgetArray data;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
};

class Dashboard: WidgetArray {

public:
  Dashboard(LOGGING &logging);
  bool load();
  String asJsonDocument();
private:
  LOGGING &logging;
};
