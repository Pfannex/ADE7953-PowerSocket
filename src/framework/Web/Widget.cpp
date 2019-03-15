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
Widget::Widget(String name, String type) : name(name), type(type){
}

//-------------------------------------------------------------------------------
//  Widget public
//-------------------------------------------------------------------------------
void Widget::fillObject(JsonObject& root){
  if (name != "")      root["name"]      = name;
  if (type != "")      root["type"]      = type;
  if (caption != "")   root["caption"]   = caption;
  if (readonly != -1)  root["readonly"]  = readonly;
  if (inputtype != "") root["inputtype"] = inputtype;
  if (direction != "") root["direction"] = direction;
  if (min != 9999)     root["min"]       = min;
  if (max != -9999)    root["max"]       = max;

  if (event != "")     root["event"]     = event;
  if (action != "")    root["action"]    = action;

  if (prefix != "")    root["prefix"]    = prefix;
  if (suffix != "")    root["suffix"]    = suffix;
}
