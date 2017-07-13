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
      return ffs.set(topic);
    }else if (topic.itemIs(2, "sysUtils")) {
      return sysUtils.set(topic);
    }else if (topic.itemIs(2, "mqtt")) {
      //return mqtt.set(topic);
    }else{
      return "NIL";
    }
//get
  }else if (topic.itemIs(1, "get")){
    if (topic.itemIs(2, "ffs")){
      return ffs.get(topic);
    }else if (topic.itemIs(2, "sysUtils")) {
      return sysUtils.get(topic);
    }else if (topic.itemIs(2, "mqtt")) {
      //return mqtt.get(topic);
    }else{
      return "NIL";
    }
  }else{
    return "NIL";
  }
}

//-------------------------------------------------------------------------------
//  API private
//-------------------------------------------------------------------------------
