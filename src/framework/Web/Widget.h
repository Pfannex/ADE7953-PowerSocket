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
    Widget(String name, String type);
    String name = "";
    String type = "";
    String caption = "";
    int readonly = -1;
    String inputtype = "";
    String direction = "";
    int min = 9999;
    int max = -9999;
    String event = "";
    String action = "";

    void fillObject(JsonObject& root);
  };

//###############################################################################
//  Widget
//###############################################################################
class WidgetList {
public:
  WidgetList(String name);
  String name = "";


};
