#include "API.h"

//###############################################################################
//  API
//###############################################################################
API::API(SysUtils& sysUtils,FFS& ffs):
     sysUtils(sysUtils),
     ffs(ffs){

}

//-------------------------------------------------------------------------------
//  API public
//-------------------------------------------------------------------------------
//...............................................................................
//  set mapping
//...............................................................................
/*String API::call(String topic, String arg){       // "foo/bar","arg1,arg2,arg3"
  TTopic tmpTopic = dissectTopic(topic, arg);
  return call(tmpTopic);
}*/

String API::call(String& topicArg){                // "foo/bar arg1,arg2,arg3"
  String topic; String arg;
  int index = -1;
  index = topicArg.indexOf(" ");
  if (index != -1){
    topic = topicArg.substring(0, index);
    topicArg.remove(0, index +1);
	arg = topicArg;
  }
  //TTopic tmpTopic = dissectTopic(topic, arg);
  //return call(tmpTopic);
}

//...............................................................................
//  set distributing
//...............................................................................
String API::call(String& strTopic, String& strArg){
/*
  sysUtils.logging.log("API", "CALL");
  String tmp = ffs.set(topic);
  sysUtils.logging.log("API", tmp);
  return tmp;
*/

  sysUtils.logging.log("call", "CLASS Topic");
  Topic topic(strTopic, strArg);
  sysUtils.logging.debugMem();
  //topic.dissectTopic(strTopic, strArg);

  sysUtils.logging.log("API", topic.asString);
  String ret = "NIL";

  if (topic.item[1] == "set"){
    if (topic.item[2] == "ffs"){
      sysUtils.logging.debugMem();
      ret = ffs.set(strTopic, strArg);
      sysUtils.logging.log("API", ret);
      return ret;
    }else if (topic.item[2] == "sysUtils") {
      //sysUtils.set(topic);
    }else if (topic.item[2] == "mqtt") {   //!!!!!!!!
      //sysUtils.set(topic);                 //
    }
  }else if (topic.item[1] == "get"){
    if (topic.item[2] == "ffs"){
      ret = ffs.get(strTopic, strArg);
      sysUtils.logging.log("API", ret);
      return ret;
    }else if (topic.item[2] == "sysUtils") {
      //String ret = sysUtils.get(topic);
      //sysUtils.logging.log("API", ret);
      //return ret;
    }
  }
}


//-------------------------------------------------------------------------------
//  API private
//-------------------------------------------------------------------------------
