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
  String type = "";
  String name = "";
  String caption = "";
  String action = "";
  String event = "";
  String inputtype = "";
  JsonArray& asArray();
};
