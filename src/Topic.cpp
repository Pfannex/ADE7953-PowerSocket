#include "Topic.h"
#include "Arduino.h"

//###############################################################################
//  data exchange Topic
//###############################################################################
Topic::Topic(String& topicsArgs) {

  int splitAt= topicsArgs.indexOf(" ");
  if(splitAt< 0) {
    // no args
    topics = new char[topicsArgs.length()];
    strcpy(topics, topicsArgs.c_str());
    args= new char[1];
    args[0]= 0;
  } else {
    // topics
    String str = topicsArgs.substring(0, splitAt);
    topics = new char[str.length()];
    strcpy(topics, str.c_str());
    // args
    str = topicsArgs.substring(splitAt+1);
    int len= str.length();
    if(!len) {
      // empty argument
      args = new char[1];
      args[0]= '\0';
    } else {
      args = new char[str.length()];
      strcpy(args, str.c_str());
    }
  }

  item = nullptr;
  arg = nullptr;
  dissectTopic(topics, args);
}

Topic::Topic(char* topics, char* args) {
  item = nullptr;
  arg = nullptr;
  dissectTopic(topics, args);
}

Topic::Topic(char* topics, int value) {
  char* args = new char[20];
  sprintf(args, "%d", value);
  item = nullptr;
  arg = nullptr;
  dissectTopic(topics, args);
  delete[] args;
}

Topic::~Topic(){
  if (item != NULL) delete[] item;
  if (arg != NULL)  delete[] arg;
  if (topics != NULL)  delete[] topics;
}

//-------------------------------------------------------------------------------
//  Topic public
//-------------------------------------------------------------------------------
//...............................................................................
//  Topic strings
//...............................................................................
String Topic::topic_asString(){
  //Serial.println("Topic::topic_asString()");
  //Serial.println(String(item[0]));
  String str = String(item[0]);
  for (int i = 1; i < countItems; i++) {
    str += "/" + String(item[i]);
  }
  return str;
}
String Topic::arg_asString(){
  String str = "";
  if (countArgs > 0) str = String(arg[0]);
  for(int i = 1; i < countArgs; i++) {
    str += "/" + String(arg[i]);
  }
  return str;
}
String Topic::asString(){
  return topic_asString() + " " + arg_asString();
}
//...............................................................................
//  dissect Topic
//...............................................................................
void Topic::dissectTopic(char* topics, char* args){
//clean up topics & args
  while (topics[strlen(topics) - 1] == '/')
    topics[strlen(topics) - 1] = '\0';

  while (args[strlen(args) - 1] == ',')
    args[strlen(args) - 1] = '\0';
  while (args[0] == ','){
    for (size_t i = 0; i < strlen(args)-1; i++) {
      args[i] = args[i+1];
    }
    args[strlen(args) - 1] = '\0';
  }

  char* ch;
//topics
  if(*topics) {
    ch= topics;
    countItems= 1;
    while(*ch) if(*ch++ == '/') countItems++;
    item = new string[countItems];

    if (countItems == 1) {
      item[0] = topics;
    }else{
      int i= 0;
      item[i] = strtok(topics, "/");
      while(item[i++] != NULL) {
        //printf("%s\n", ptr);
     	  item[i] = strtok(NULL, "/");
      }
    }
  }
//args
  if(*args) {
    ch= args;
    countArgs= 1;
    if (isValidJson(String(args))) {
      arg = new string[1];
      arg[0] = args;
    }else{
      while(*ch) if(*ch++ == ',') countArgs++;
      arg = new string[countArgs];

      if (countArgs == 1) {
        arg[0] = args;
      }else{
        int i= 0;
        arg[i] = strtok(args, ",");
        while(arg[i++] != NULL) {
     	  arg[i] = strtok(NULL, ",");
        }
      }
    }
  }
}

//...............................................................................
//  delete TopicItem in return String
//...............................................................................
String Topic::modifyTopic(int index){
  String str = "";

  for (int i = 0; i < countItems; i++) {
    if (i != index) {
      str += String(item[i]);
      if (i != countItems-1) {
        str += "/";
      }
    }
  }
  return str;
}

//...............................................................................
//  compare topic.item by index with string
//...............................................................................
bool Topic::itemIs(int index, string topicName){
  if (index < countItems)
    return !strcmp(item[index], topicName);
  else
    return false;
}

//...............................................................................
//  compare topic.arg by index with string
//...............................................................................
bool Topic::argIs(int index, const string value){
  if (index < countArgs)
    return !strcmp(arg[index], value);
  else
    return false;
}

//-------------------------------------------------------------------------------
//  Topic private
//-------------------------------------------------------------------------------
//...............................................................................
//  check for valid JSON-String
//...............................................................................
bool Topic::isValidJson(String root){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(root);
  return json.success();
}

//...............................................................................
//  print Topic
//...............................................................................
void Topic::printTopic(){
  Serial.println("............................................");
  for (int i = 0; i < countItems; i++) {
    Serial.println(item[i]);
  }
  for (int i = 0; i < countArgs; i++) {
    Serial.println(arg[i]);
  }
  Serial.println("............................................");
}

//###############################################################################
//  Topic Queue
//###############################################################################

TopicQueue::TopicQueue() {

}

void TopicQueue::clear() {
  while(count) get();
}

void TopicQueue::put(String& topicsArgs) {

  if(count>= TOPIC_QUEUE_MAX) return;
  // create a new element
  //Serial.println("queue: put "+topicsArgs);
  element_t* e= new element_t;
  e->topicsArgs= new char[topicsArgs.length()+1]; // +1 for trailing 0
  strcpy(e->topicsArgs, topicsArgs.c_str());
  // prepend to tail
  e->next= tail;
  if(count) {
    // if the queue has elements, backlink previous tail to new element
    tail->prev= e;
  } else {
    // else the new element is also made the head
    head= e;
  }
  // the new element is made the tail
  tail= e;
  // increase count
  count++;
  //Serial.println("queue: put done");
}

void TopicQueue::put(const char* topicsArgs) {
  String t= String(topicsArgs);
  put(t);
}

String TopicQueue::get() {

  //Serial.println("queue: get begin");
  if(count) {
    element_t* e= head;
    String topicsArgs= String(e->topicsArgs);
    head= head->prev;
    delete[] e;
    count--;
    //Serial.println("queue: get "+topicsArgs);
    return topicsArgs;
  } else {
    return String(""); // if you get here then you have made an programming error
  }
}
