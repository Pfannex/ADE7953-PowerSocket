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
String API::call(String topic, String arg){       // "foo/bar","arg1,arg2,arg3"
  TTopic tmpTopic = dissectTopic(topic, arg);
  call(tmpTopic);
}

String API::call(String topicArg){                // "foo/bar arg1,arg2,arg3"
  String topic; String arg;
  int index = -1;
  index = topicArg.indexOf(" ");
  if (index != -1){
    topic = topicArg.substring(0, index);
    topicArg.remove(0, index +1);
	arg = topicArg;
  }
  TTopic tmpTopic = dissectTopic(topic, arg);
  call(tmpTopic);
}

//...............................................................................
//  set distributing
//...............................................................................
String API::call(TTopic topic){
  sysUtils.logging.log("API", topic.asString);

  if (topic.item[1] == "set"){
    if (topic.item[2] == "ffs"){
      ffs.set(topic);
    }else if (topic.item[2] == "sysUtils") {
      sysUtils.set(topic);
    }
  }else if (topic.item[1] == "get"){
    if (topic.item[2] == "ffs"){
      String ret = ffs.get(topic);
      sysUtils.logging.log("API", ret);
      return ret;
    }else if (topic.item[2] == "sysUtils") {
      String ret = sysUtils.get(topic);
      sysUtils.logging.log("API", ret);
      return ret;
    }
  }
}

String API::set(String topic, String arg) {
  TTopic tmpTopic = dissectTopic(topic, arg);
  return set(tmpTopic);

}

String API::set(TTopic topic) {
  if (topic.item[0] == "ffs"){
    ffs.set(topic);
  }

}


String API::get(String topic, String arg) {
  TTopic tmpTopic = dissectTopic(topic, arg);
  return get(tmpTopic);
}

String API::get(TTopic topic) {
  if (topic.item[0] == "ffs"){
    return ffs.get(topic);
  }

}


//...............................................................................
//  dissect Topic
//...............................................................................

// remove top level from topic
String API::stripTopic(TTopic topic) {
  String topItem;
  if(!topic.countTopics) return("");
  topItem= topic.item[0];
  for(int i= 1; i< topic.countTopics; i++) {
    topic.item[i-1]= topic.item[i];
  }
  topic.countTopics--;
  return topItem;
}

TTopic API::dissectTopic(String topic, String arg){

//Prüfung ob arg ein jsonString ist!!

  TTopic tmpTopic;
  tmpTopic.asString = topic + " | " + arg;
  tmpTopic.itemAsString = topic;
  tmpTopic.argAsString = arg;

  tmpTopic.countTopics = 0;
  tmpTopic.countArgs = 0;
  if (topic.startsWith("/")) topic.remove(0, 1);
  if (topic.endsWith("/")) topic.remove(topic.length()-1,topic.length());

//topic
  int index = -1; int i = 0;
  do{
    index = topic.indexOf("/");
    if (index != -1){
      tmpTopic.item[i] = topic.substring(0, index);
	    topic.remove(0, index +1);
	    i++;
	  }else{
	    index = -1;
		  tmpTopic.item[i] = topic;
		  i++;
	  }
  } while (index != -1);
  tmpTopic.countTopics = i;

//argument
  if (ffs.isValidJson(arg)){
    tmpTopic.arg[0] = arg;   //don't split if arg ist JSON-String
    tmpTopic.countArgs = 1;
  }else{
  index = -1; i = 0;
  do{
    index = arg.indexOf(",");
    if (index != -1){
      tmpTopic.arg[i] = arg.substring(0, index);
	    arg.remove(0, index +1);
	    i++;
	  }else{
  	  index = -1;
  		tmpTopic.arg[i] = arg;
  		i++;
  	}
  } while (index != -1);
  tmpTopic.countArgs = i;
  }
  //printTopic(tmpTopic);
  return tmpTopic;
}

//...............................................................................
//  delete TopicItem
//...............................................................................
String API::deleteTopicItem(String topic, int item){
  return topic;
}

//-------------------------------------------------------------------------------
//  API private
//-------------------------------------------------------------------------------

//...............................................................................
//  print Topic
//...............................................................................
void API::printTopic(TTopic topic){
  Serial.println("............................................");
  for (int i = 0; i < topic.countTopics; i++) {
    Serial.println(topic.item[i]);
  }
  for (int i = 0; i < topic.countArgs; i++) {
    Serial.println(topic.arg[i]);
  }
  Serial.println("............................................");
}
