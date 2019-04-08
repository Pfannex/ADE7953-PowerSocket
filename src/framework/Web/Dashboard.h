#include "framework/Utils/Logger.h"
#include <Arduino.h>
#include <ArduinoJson.h>

// https://arduinojson.org/v5/doc/

#define JSONBUFFERSIZE 4096
#define DASHBOARDFILENAME "/customDevice/dashboard.json"

//
// Widget
//

class Widget {

private:
  String getProperty(JsonObject &O, const char *property);
  void setProperty(JsonObject &O, const char *property, String value);

public:
  Widget();
  Widget(String &type);
  Widget(const char* type);
  virtual ~Widget();
  String name; // must be unique across the whole structure
  String type;
  String value;
  String caption;
  String icon;
  String action;
  String event;
  String inputtype;
  String readonly;
  String min;
  String max;
  String prefix;
  String suffix;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
};

//
// WidgetArray
//

class WidgetArray {
public:
  ~WidgetArray();

  virtual JsonArray &serialize(DynamicJsonBuffer &);
  virtual void deserialize(JsonArray &);

  // removes the named widget from anywhere in the structure
  bool removeWidget(String &name);
  // add the widget at given position
  // position is counted from the end if negative
  virtual Widget *insertWidget(String &type, int position = -1);
  virtual Widget *insertWidget(const char* type, int position = -1);
  // add the widget to the named group at given position
  virtual Widget *insertWidget(String &type, String &group, int position = -1);
  virtual Widget *insertWidget(const char* type, const char* group, int position = -1);

  /*
  usage:
  // append a push button to the dashboard

  // append a group named "roster" to dashboard
  WidgetGroup* g= dashboard.insertWidget("group");
  g->name= "roster";
  // append a text widget to the group "roster"
  Widget* w= dashboard.insertWidget("text", "roster");
  */

private:
  std::vector<Widget *> widgets;
  // http://www.dorodnic.com/blog/2014/03/29/abstract-factory/
  Widget *createWidget(JsonObject &) const; // abstract factory
  Widget *createWidget(String &) const;
};

//
// WidgetGroup
//

class WidgetGroup : public Widget {
private:
  WidgetArray data;
public:
  WidgetGroup();
  virtual ~WidgetGroup();
  // add the widget to the named group at given position
  virtual Widget *insertWidget(String &type, String &group, int position = -1);
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
  // removes the named widget from anywhere in the structure
  bool removeWidget(String &name);
};

//
// WidgetGrid
//
class WidgetGrid : public Widget {
private:
    std::vector<WidgetArray*> data;
public:
    WidgetGrid();
    virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
    virtual void fromJsonObject(JsonObject &);
    WidgetArray* addRow();
};

//
// WidgetControlGroup
//

struct WidgetControlGroupElement {
    String label;
    String value;
};

class WidgetControlGroup : public Widget {
private:
  std::vector<WidgetControlGroupElement> data;
public:
  WidgetControlGroup();
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
  void appendElement(String&, String&);
  void appendElement(const char*, const char*);
};


//
// Dashboard
//

class Dashboard : public WidgetArray {

public:
  Dashboard(LOGGING &logging);
  bool load();
  String asJsonDocument();

private:
  LOGGING &logging;
};
