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

protected:
  String getProperty(JsonObject &O, const char *property);
  void setProperty(JsonObject &O, const char *property, String value);

public:
  Widget();
  Widget(String &type);
  Widget(const char *type);
  virtual ~Widget();
  // properties
  String name; // must be unique across the whole structure
  String type; 
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
};

//
// WidgetCaptioned
//

class WidgetCaptioned : public Widget {

public:
  WidgetCaptioned(const char*);
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
  // properties
  String caption;
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
  virtual Widget *insertWidget(const char *type, int position = -1);
  // add the widget to the named group at given position
  virtual Widget *insertWidget(String &type, String &group, int position = -1);
  virtual Widget *insertWidget(const char *type, const char *group,
                               int position = -1);

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

class WidgetGroup : public WidgetCaptioned {
private:
  WidgetArray data;

public:
  WidgetGroup();
  virtual ~WidgetGroup();
  // properties
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
  std::vector<WidgetArray *> data;

public:
  WidgetGrid();
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
  WidgetArray *addRow();
};


//
// WidgetAction
//

class WidgetAction : public WidgetCaptioned {
public:
  WidgetAction(const char*);
  // properties
  String action;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
};

//
// WidgetButton
//

class WidgetButton : public WidgetAction {
public:
  WidgetButton();
  // properties
  String icon;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
};

//
// WidgetInput
//

class WidgetInput : public WidgetAction {
public:
  WidgetInput(const char*);
  // properties
  String value;
  String readonly;
  String event;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
};

//
// WidgetControlGroup
//

struct WidgetControlGroupElement {
  String label;
  String value;
};

class WidgetControlGroup : public WidgetInput {
private:
  std::vector<WidgetControlGroupElement> data;

public:
  WidgetControlGroup();
  // properties
  String direction;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
  void appendElement(String &, String &);
  void appendElement(const char *, const char *);
};


//
// WidgetText
//

class WidgetText : public WidgetInput {
public:
  WidgetText();
  // properties
  String inputtype;
  String prefix;
  String suffix;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
};

//
// WidgetCheckbox
//

class WidgetCheckbox : public WidgetInput {
public:
  WidgetCheckbox();
};

//
// WidgetSlider
//

class WidgetSlider : public WidgetInput {
public:
  WidgetSlider();
  // properties
  String min;
  String max;
  virtual void toJsonObject(DynamicJsonBuffer &, JsonObject &);
  virtual void fromJsonObject(JsonObject &);
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
