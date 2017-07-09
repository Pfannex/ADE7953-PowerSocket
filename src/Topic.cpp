#include "Topic.h"

//###############################################################################
//  data exchange Topic
//###############################################################################
Topic::Topic(String topic_asString, String arg_asString):
       topic_asString(topic_asString),
       arg_asString(arg_asString){

  item = nullptr;
  arg = nullptr;
  dissectTopic();
}
Topic::~Topic(){
  if (item != NULL) delete[] item;
  if (arg != NULL)  delete[] arg;
}

//-------------------------------------------------------------------------------
//  Topic public
//-------------------------------------------------------------------------------
//...............................................................................
//  dissect Topic
//...............................................................................
void Topic::dissectTopic(){
  Serial.println("Topic::dissectTopic()");

  if (topic_asString.startsWith("/")) topic_asString.remove(0, 1);
  if (topic_asString.endsWith("/")) topic_asString.remove(topic_asString.length()-1,topic_asString.length());

  if (topic_asString.length() != 0) countTopics++;
  for (size_t i = 0; i < topic_asString.length(); i++) {
    if (topic_asString.substring(i, i+1) == "/") countTopics++;
  }
  item = new string[countTopics];
  Serial.println(countTopics);

  if (arg_asString.length() != 0) countArgs++;
  for (size_t i = 0; i < arg_asString.length(); i++) {
    if (arg_asString.substring(i, i+1) == ",") countArgs++;
  }
  arg = new string[countArgs];
  Serial.println(countArgs);



  item[0] = "Item0";
  Serial.println(item[0]);


/*
  asString = topic_asString + " | " + arg_asString;

  countTopics = 0;
  countArgs = 0;
  if (topic_asString.startsWith("/")) topic_asString.remove(0, 1);
  if (topic_asString.endsWith("/")) topic_asString.remove(topic_asString.length()-1,topic_asString.length());

//topic
  int index = -1; int i = 0;
  do{
    index = topic_asString.indexOf("/");
    if (index != -1){
      item[i] = topic_asString.substring(0, index);
	    topic_asString.remove(0, index +1);
	    i++;
	  }else{
	    index = -1;
		  item[i] = topic_asString;
		  i++;
	  }
  } while (index != -1);
  countTopics = i;

//argument
//  if (ffs.isValidJson(arg)){
//    arg[0] = arg;   //don't split if arg ist JSON-String
//    countArgs = 1;
//  }else{
  index = -1; i = 0;
  do{
    index = arg_asString.indexOf(",");
    if (index != -1){
      arg[i] = arg_asString.substring(0, index);
	    arg_asString.remove(0, index +1);
	    i++;
	  }else{
  	  index = -1;
  		arg[i] = arg_asString;
  		i++;
  	}
  } while (index != -1);
  countArgs = i;
//  }
  //printTopic(tmpTopic);
  //return tmpTopic;
  */
}

//...............................................................................
//  delete TopicItem
//...............................................................................
String Topic::deleteTopicItem(int item){
  //for (size_t i = 0; i < count; i++) {
    /* code */
  //}

  //String returnTopic = "Node52/ffs/cfg"; //tmpTopic.item[0];
  //for (int i = 2; i < tmpTopic.countTopics; i++) {
  //    returnTopic += "/" + tmpTopic.item[i];
  //}
  //return Tpoic oder lieber ein allgemeines Topic "RESULT"??

  return "";
}

//-------------------------------------------------------------------------------
//  Topic private
//-------------------------------------------------------------------------------
