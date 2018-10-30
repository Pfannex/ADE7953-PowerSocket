#include <ArduinoJson.h>
#include "framework/Utils/Logger.h"

// https://arduinojson.org/v5/doc/

#define JSONBUFFERSIZE 4096
#define DASHBOARDFILENAME "/customDevice/dashboard.json"


class Widget  {

public:
  String name;
  void toJsonObject(JsonObject&);
  void fromJsonObject(JsonObject&);

};


class WidgetArray : public std::vector<Widget> {

};

class Dashboard {

public:
  Dashboard(LOGGING& logging);
  bool load();
  void serialize(String&);
  void deserialize(JsonArray&);
  String asJsonDocument();
  WidgetArray widgets;

private:
  Widget createWidget(JsonObject&);
  LOGGING& logging;
};
