#pragma once
  #include <ArduinoJson.h>
  #include <Arduino.h>
  #include "framework/Utils/Debug.h"

//###############################################################################
//  Widget
//###############################################################################
//links
//https://arduinojson.org/v5/assistant/
//https://arduinojson.org/v5/doc/

class Text {
public:
  Text(String name);
  String name = "";
  String type = "";
  String caption = "";
  int readonly = 1;
  String action = "";
  String event = "";
  String inputtype = "";
  JsonObject& asObject();
};
