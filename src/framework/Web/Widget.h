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

  class Widget {
  public:
    Widget(String name);
    String name = "";
    String type = "";
    String caption = "";
    int readonly = 1;
    String inputtype = "";
    String direction = "";
    int min = 1;
    int max = 1;
    String event = "";
    String action = "";

    JsonObject& asObject();
  };

//###############################################################################
//  Widget
//###############################################################################
class WidgetList {
public:
  WidgetList(String name);
  String name = "";


};
