#include "framework/Web/Widget.h"

//###############################################################################
//  WidgetList
//###############################################################################

WidgetList::WidgetList(String name) : name(name){}

//-------------------------------------------------------------------------------
//  WidgetList public
//-------------------------------------------------------------------------------





//###############################################################################
//  Widget
//###############################################################################

Widget::Widget(String name) : name(name){
}

//-------------------------------------------------------------------------------
//  Widget public
//-------------------------------------------------------------------------------
JsonObject& Widget::asObject(){
  const size_t bufferSize = JSON_OBJECT_SIZE(10);
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.createObject();
    root["name"]      = name;
    root["type"]      = type;
    root["caption"]   = caption;
    root["readonly"]  = readonly;
    root["inputtype"] = inputtype;
    root["direction"] = direction;
    root["min"]       = min;
    root["max"]       = max;

    root["event"]     = event;
    root["action"]    = action;
  return root;
}
