#include "framework/Web/Widget.h"

//###############################################################################
//  Widget
//###############################################################################

Text::Text(String name) : name(name){}

//-------------------------------------------------------------------------------
//  FFS public
//-------------------------------------------------------------------------------
JsonArray& Text::asArray(){
  //const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(6);
  //DynamicJsonBuffer jsonBuffer(bufferSize);
  DynamicJsonBuffer jsonBuffer;
  JsonArray& root = jsonBuffer.createArray();

  JsonObject& root_0 = root.createNestedObject();
  root_0["type"] = type;
  root_0["name"] = name;
  root_0["caption"] = caption;
  root_0["action"] = action;
  root_0["event"] = event;
  root_0["inputtype"] = inputtype;
  return root;
  //root.printTo(Serial);
}
