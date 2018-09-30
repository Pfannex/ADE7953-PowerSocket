#include "framework/Web/Widget.h"

//###############################################################################
//  Widget
//###############################################################################

Text::Text(String name) : name(name){}

//-------------------------------------------------------------------------------
//  FFS public
//-------------------------------------------------------------------------------
JsonObject &Text::asObject(){
  const size_t bufferSize = JSON_OBJECT_SIZE(7);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  root["name"]      = name;
  root["type"]      = type;
  root["caption"]   = caption;
  root["readonly"]  = readonly;
  root["action"]    = action;
  root["event"]     = event;
  root["inputtype"] = inputtype;
  return root;

  //root.printTo(Serial);
}
