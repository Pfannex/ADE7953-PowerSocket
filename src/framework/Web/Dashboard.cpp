#include "framework/Web/Dashboard.h"

//###############################################################################
//  Widget
//###############################################################################

Dashboard::Dashboard(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
                    :logging(logging), topicQueue(topicQueue), ffs(ffs){

}

//-------------------------------------------------------------------------------
//  FFS public
//-------------------------------------------------------------------------------
//...............................................................................
// buildDashboard
//...............................................................................
void Dashboard::buildDashboard(){

}



void Dashboard::sayHello(){

  Text myText("Widget_1");
    myText.type      = "text";
    myText.caption   = "NEWTime";
    myText.readonly  = 1;
    myText.event     = "~/event/clock/time";
    myText.inputtype = "datetime";

  String str;
  myText.asArray().prettyPrintTo(Serial);
  myText.asArray().printTo(str);
  ffs.webCFG.root = str;
  //ffs.webCFG.saveFile();
}

/*
Text text1("Clock");
  text1.type      = "text";
  text1.caption   = "NEW Time!";
  text1.readonly  = 1;
  text1.action    = "";
  text1.event     = "~/event/clock/time";
  text1.inputtype = "datetime";

  Serial.println("newObject");
  text1.asObject().prettyPrintTo(Serial);
  Serial.println("");
  Serial.println("-------------------------------------");


  StaticJsonBuffer<2000> jsonBuffer;

  // create an object
  //JsonObject& object1 = jsonBuffer.createObject();
  //DynamicJsonBuffer jsonBuffer;
  //object1 = text1.asObject();

  Serial.println("newObject");
  //rootObject.prettyPrintTo(Serial);
  Serial.println("");
  Serial.println("-------------------------------------");

  ffs.webCFG.set_toRoot("", text1.asObject());


  topicQueue.put("~/event/ui/dashboardChanged");
*/
