#include "Topic.h"

//###############################################################################
//  data exchange Topic
//###############################################################################
Topic::Topic(String& topicsArgs) {
//topics
  String str = topicsArgs.substring(0, topicsArgs.indexOf(" "));
  topics= new char[str.length()];
  strcpy(topics, str.c_str());
//args
  str = topicsArgs.substring(topicsArgs.indexOf(" ")+1);
  args = new char[str.length()];
  strcpy(args, str.c_str());

  item = nullptr;
  arg = nullptr;
  dissectTopic(topics, args);
}

Topic::Topic(char* topics, char* args) {
  item = nullptr;
  arg = nullptr;
  dissectTopic(topics, args);
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
String topic_asString(){
  return "";
}
String arg_asString(){
  return "";

}
String asString(){
  return "";

}
//...............................................................................
//  dissect Topic
//...............................................................................
void Topic::dissectTopic(char* topics, char* args){
  Serial.println("Topic::dissectTopic()");

  //String strTopics = String(topics);
  //char* chrTopics
  //char* x;
  //strcpy(x, strTopics.c_str());

  //String strArgs = String(args);
  //strcpy(tmpTopics, args);

  //Serial.println(String(topics));
  //Serial.println(String(args));
  //Serial.println(strTopics);

  char* ch;

  if(*topics) {
    ch= topics;
    countItems= 1;
    while(*ch) if(*ch++ == '/') countItems++;
    //Serial.println(countItems);
    item = new string[countItems];

    int i= 0;
    item[i] = strtok(topics, "/");
    while(item[i++] != NULL) {
      //printf("%s\n", ptr);
     	item[i] = strtok(NULL, "/");
    }
  }

  if(*args) {
    ch= args;
    countArgs= 1;
    //wenn {....} dann jsonString!!
    if (isValidJson(String(args))) {
      Serial.println("is JSON");
      arg = new string[1];
      arg[0] = args;
    }else{
      while(*ch) if(*ch++ == ',') countArgs++;
      //Serial.println(countArgs);
      arg = new string[countArgs];
      int i= 0;
      arg[i] = strtok(args, ",");
      while(arg[i++] != NULL) {
     	  arg[i] = strtok(NULL, ",");
      }
    }
  }

/*
  for (size_t i = 0; i < countItems; i++) {
    Serial.println(item[i]);
  }
  for (size_t i = 0; i < countArgs; i++) {
    Serial.println(arg[i]);
  }
*/

  /*

  ptr = strtok(args, ",");
  while(ptr != NULL) {
   	ptr = strtok(NULL, ",");
    countArgs++;
  }
*/
  //countItems = 6;
  //countArgs = 3;
  //Serial.println(countArgs);
  //arg = new string[countArgs];



  //Serial.println(topics);
  //Serial.println(args);

  //trcpy(topics, strTopics.c_str());

  //char delimiter[] = "/";
  // initialisieren und ersten Abschnitt erstellen
  //char *ptr;
  //ptr = strtok(topics, delimiter);
  //int i = 0;
  //while(ptr != NULL){
  //  printf("Item %d, %s\n", i, ptr);
    //printf("Item %s\n", ptr);
    //strcpy(item[i], ptr);
  //  i++;
  	// naechsten Abschnitt erstellen
  // 	ptr = strtok(NULL, delimiter);
  //}

  //for (size_t i = 0; i < countItems; i++) {
    //Serial.println(item[i]);
  //}

/*  topic_asString = "abc";
  arg_asString = "def";
  asString = "ghi";

  Serial.println("Topic::var end");*/







  //Serial.println("Topic::delete");
  //delete[] chrTopic;
  //delete[] chrArg;
  //Serial.println("Topic::delete end");


  /*
    char* chrTopic = new char(topic_asString.length());
    strcpy(chrTopic, topic_asString.c_str());
    char* chrArg = new char(topic_asString.length());
    strcpy(chrArg, topic_asString.c_str());
  */


/*
//entfällt wenn char übergeben wird
  char* chrTopic;
  chrTopic = new char[topic_asString.length()];
  strcpy(chrTopic, topic_asString.c_str());


  char* chrArg;
  chrArg = new char[arg_asString.length()];
  strcpy(chrArg, arg_asString.c_str());
  Serial.println(strlen(chrTopic));
  Serial.println(chrTopic);
  Serial.println(strlen(chrArg));
  Serial.println(chrArg);


  string s = "123";
*/

/*
  for (size_t i = 0; i < strlen(chrTopic); i++) {
    if (chrTopic[i] == "/"){
      countTopics++;
    }else{
      strcpy(item[countTopics], chrTopic[i]);
    }
  }

  for (size_t i = 0; i < countTopics; i++) {
    Serial.println(item[i]);
  }
*/
/*
  if (topic_asString.startsWith("/")) topic_asString.remove(0, 1);
  if (topic_asString.endsWith("/")) topic_asString.remove(topic_asString.length()-1,topic_asString.length());

  if (topic_asString.length() != 0) countTopics++;
  for (size_t i = 0; i < topic_asString.length(); i++){
    if (topic_asString.substring(i, i+1) == "/") countTopics++;
  }
  item = new string[countTopics];
  //Serial.println(countTopics);
  //Serial.println(topic_asString);

  if (arg_asString.length() != 0) countArgs++;
  for (size_t i = 0; i < arg_asString.length(); i++){
    if (arg_asString.substring(i, i+1) == ",") countArgs++;
  }
  arg = new string[countArgs];
  //Serial.println(countArgs);
  //Serial.println(arg_asString);
*/


  //topic
/*  int index = -1;
  String str = topic_asString;
  char chr[500] = "";
  for (size_t i = 0; i < countTopics; i++) {
    index = str.indexOf("/");
    if (index != -1){
      strcpy(chr, str.substring(0, index).c_str());
      item[i] = chr;
  	  str.remove(0, index +1);
  	}else{
  	   index = -1;
       strcpy(chr, str.c_str());
  		 item[i] = chr;
  	}
  }
  printTopic();*/


  //item[0] = "Item0";
  //Serial.println(item[0]);


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

bool Topic::itemIs(int index, string topicName){
  return !strcmp(item[index], topicName);
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
  //for (int i = 0; i < countArgs; i++) {
    //Serial.println(arg[i]);
  //}
  Serial.println("............................................");
}
