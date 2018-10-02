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
// set Dashboard
//...............................................................................
void Dashboard::setDashboard(int number){


  ffs.webCFG.root = buildDashboard();
  //ffs.webCFG.saveFile();
}

//...............................................................................
// build Dashboard
//...............................................................................
String Dashboard::buildDashboard(){

  return page_main();
}


//...............................................................................
// page_main
//...............................................................................
String Dashboard::page_main(){

  Widget w1("Widget_1");
    w1.type      = "text";
    w1.caption   = "NEWTime";
    w1.readonly  = 1;
    w1.event     = "~/event/clock/time";
    w1.inputtype = "datetime";

  w1.asObject().prettyPrintTo(Serial);
  Serial.println("-------------------------");

  Widget w2("Widget_2");
    w2.type      = "text";
    w2.caption   = "NEWNEWTime";
    w2.readonly  = 1;
    w2.event     = "~/event/clock/time";
    w2.inputtype = "datetime";

  w2.asObject().prettyPrintTo(Serial);
  Serial.println("-------------------------");

  DynamicJsonBuffer jsonBuffer;
  JsonArray& root = jsonBuffer.createArray();
  root.add(w1.asObject());
  root.add(w2.asObject());

  String rootStr;
  root.prettyPrintTo(Serial);
  Serial.println("-------------------------");
  root.printTo(rootStr);
  return rootStr;
}
