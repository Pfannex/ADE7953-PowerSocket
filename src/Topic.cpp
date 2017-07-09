#include "Topic.h"

//###############################################################################
//  data exchange Topic
//###############################################################################
Topic::Topic(){

}

//-------------------------------------------------------------------------------
//  Topic public
//-------------------------------------------------------------------------------
//...............................................................................
//  dissect Topic
//...............................................................................
void Topic::dissectTopic(String strTopic, String strArg){
  asString = strTopic + " | " + strArg;
  itemAsString = strTopic;
  argAsString = strArg;

  countTopics = 0;
  countArgs = 0;
  if (strTopic.startsWith("/")) strTopic.remove(0, 1);
  if (strTopic.endsWith("/")) strTopic.remove(strTopic.length()-1,strTopic.length());

//topic
  int index = -1; int i = 0;
  do{
    index = strTopic.indexOf("/");
    if (index != -1){
      item[i] = strTopic.substring(0, index);
	    strTopic.remove(0, index +1);
	    i++;
	  }else{
	    index = -1;
		  item[i] = strTopic;
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
    index = strArg.indexOf(",");
    if (index != -1){
      arg[i] = strArg.substring(0, index);
	    strArg.remove(0, index +1);
	    i++;
	  }else{
  	  index = -1;
  		arg[i] = strArg;
  		i++;
  	}
  } while (index != -1);
  countArgs = i;
//  }
  //printTopic(tmpTopic);
  //return tmpTopic;
}

//...............................................................................
//  delete TopicItem
//...............................................................................
String Topic::deleteTopicItem(String topic, int item){
  return topic;
}

//-------------------------------------------------------------------------------
//  Topic private
//-------------------------------------------------------------------------------
