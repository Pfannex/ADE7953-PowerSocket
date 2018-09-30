#pragma once
  #include <ArduinoJson.h>
  #include <Arduino.h>
  #include "framework/Utils/Debug.h"

  //links
  //https://arduinojson.org/v5/assistant/
  //https://arduinojson.org/v5/doc/

  //###############################################################################
  //  Widget
  //###############################################################################

  class Text {
  public:
    Text(String name);
    String name = "test";
    String type = "";
    String caption = "";
    int readonly = 1;
    String event = "";
    String inputtype = "";
    JsonArray& asArray();
    String json();
  };

//###############################################################################
//  Widget
//###############################################################################
class WidgetList {
public:
  //WidgetList(String name);
  WidgetList();
  String name = "";


};
