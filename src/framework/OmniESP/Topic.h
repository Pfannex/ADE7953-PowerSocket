#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "framework/Utils/Strings.h"

//###############################################################################
//  Topic
//###############################################################################

#define TOPIC_OK String("ok")
#define TOPIC_NO String("no such topic")

class Topic{
public:
  Topic(String& topicsArgs);         // "foo/bar arg1,arg2,arg3"
  Topic(string topicsArgs);          // "foo/bar arg1,arg2,arg3"
  Topic(string topics, string args); // "foo/bar","arg1,arg2,arg3"
  Topic(string topics, int value);   // "foo/bar",1
  ~Topic();

  String topic_asString();
  char* topic_asCStr(); // do not free!
  String arg_asString();
  char* arg_asCStr(); // do not free!
  String asString();

  int getItemCount();
  string getItem(unsigned int index);
  long getItemAsLong(unsigned int index);
  int getArgCount();
  string getArg(unsigned int index);
  // Function declarations that differ only in the return type cannot be
  // overloaded. We thus choose another name.
  // detects and converts hex (0x) and octal (0..) values
  long getArgAsLong(unsigned int index);
  float getArgAsFloat(unsigned int index);

  String modifyTopic(unsigned int index);
  void setItem(unsigned int index, const string topicName);
  bool itemIs(unsigned int index, const string topicName);
  bool argIs(unsigned int index, const string value);

private:
  strings item;
  strings arg;
  void initTopic(string topicsArgs);
  void dissectTopic(string topics, string arg);
  bool isValidJson(String& root);
  bool looksLikeJson(string s);
  // splits str into tokens with memory allocations, returns number of tokens
  int tokenize(strings &tokens, string str, string delimiters);
};

//###############################################################################
//  Topic Callbacks
//###############################################################################

#include <functional>
typedef std::function<void(const time_t, Topic&)> TopicFunction;

//###############################################################################
//  Topic Queue
//###############################################################################

#define TOPIC_QUEUE_MAX 20

struct element_t {
  struct element_t* prev;
  struct element_t* next;
  char* topicsArgs;
};

class TopicQueue {
public:
    TopicQueue();
    void clear();  // clear the Queue
    void put(String& topicsArgs);
    void put(const char*);
    void put(const char *topics, int arg);
    void put(const char *topics, float arg, const char* format= "%f");
    String get();
    int count= 0;
private:
    void put(element_t* e);
    element_t* head= NULL; // we get from head
    element_t* tail= NULL; // we put to tail
};
