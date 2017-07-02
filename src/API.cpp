#include "API.h"

//###############################################################################
//  API
//###############################################################################
API::API(FFS& ffs):
         ffs(ffs){
}

//-------------------------------------------------------------------------------
//  API public
//-------------------------------------------------------------------------------
//...............................................................................
//  set mapping
//...............................................................................
bool API::call(String topic, String arg){       // "foo/bar","arg1,arg2,arg3"
  TTopic tmpTopic = dissectTopic(topic, arg);
  call(tmpTopic);
}

bool API::call(String topicArg){                // "foo/bar arg1,arg2,arg3"
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
bool API::call(TTopic topic){        //e.g. "Node52/set/ffs/cfg/item/webUser Klaus"
  String topItem= stripTopic(topic);
  if (topItem == "set") {      //e.g. "Node52/set/ffs/cfg/saveFile"
    set(topic);
  } else if(topItem == "get") {
    get(topic);
  } else {
    //logging.error('no such item: '+topItem)
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
  TTopic tmpTopic;

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

  return tmpTopic;
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
