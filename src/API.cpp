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
//  API call distributing
//...............................................................................
String API::call(Topic& topic){
  String ret = "NIL";

//set
  if (topic.itemIs(1, "set")){
    if (topic.itemIs(2, "ffs")){
      //sysUtils.logging.debugMem();
      ret = ffs.set(topic);
      //sysUtils.logging.log("API", ret);
      return ret;
    }else if (topic.itemIs(2, "sysUtils")) {
        ret = sysUtils.set(topic);
      return ret;
    }else if (topic.itemIs(2, "mqtt")) {
      //return mqtt.set(topic);
    }
//get
  }else if (topic.itemIs(1, "get")){
    if (topic.itemIs(2, "ffs")){
      ret = ffs.get(topic);
      //sysUtils.logging.log("API", ret);
      return ret;
    }else if (topic.itemIs(2, "sysUtils")) {
      ret = sysUtils.get(topic);
      //sysUtils.logging.log("API sysUtils", ret);
      return ret;
    }else if (topic.itemIs(2, "mqtt")) {
      //return mqtt.get(topic);
    }
  }
}

//-------------------------------------------------------------------------------
//  API private
//-------------------------------------------------------------------------------
