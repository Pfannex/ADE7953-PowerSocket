#include "framework/Web/Widget.h"

//###############################################################################
//  WidgetList
//###############################################################################

//WidgetList::WidgetList(String name) : name(name){}
WidgetList::WidgetList() {

}

//-------------------------------------------------------------------------------
//  WidgetList public
//-------------------------------------------------------------------------------





//###############################################################################
//  Widget
//###############################################################################

Text::Text(String name) : name(name){}

//-------------------------------------------------------------------------------
//  Text public
//-------------------------------------------------------------------------------
JsonArray& Text::asArray(){
  //const size_t bufferSize = JSON_OBJECT_SIZE(7);
  DynamicJsonBuffer jsonBuffer;
  JsonArray& root = jsonBuffer.createArray();

  JsonObject& root_0 = root.createNestedObject();
  root_0["name"]      = name;
  root_0["type"]      = type;
  root_0["caption"]   = caption;
  root_0["readonly"]  = readonly;
  root_0["event"]     = event;
  root_0["inputtype"] = inputtype;

/*
  JsonObject& root_1 = root.createNestedObject();
  root_1["name"]      = name;
  root_1["type"]      = type;
  root_1["caption"]   = caption;
  root_1["readonly"]  = readonly;
  root_1["event"]     = event;
  root_1["inputtype"] = inputtype;
*/
  return root;
}

String Text::json(){

  //root.printTo(Serial);
}
