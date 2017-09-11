#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

//###############################################################################
//  Topic
//###############################################################################

#define TOPIC_OK String("ok")
#define TOPIC_NO String("no such topic")

typedef const char* string;
class Topic{
public:
  Topic(String& topicsArgs);       // "foo/bar arg1,arg2,arg3"
  Topic(char* topics, char* args); // "foo/bar","arg1,arg2,arg3"
  Topic(char* topics, int value); // "foo/bar",1
  ~Topic();

  string* item;
  string* arg;
  char* topics;
  char* args;
  int countItems = 0;
  int countArgs = 0;
  String topic_asString();
  String arg_asString();
  String asString();

  void dissectTopic(char* topics, char* arg);
  String modifyTopic(int index);
  bool itemIs(int index, const string topicName);
  bool argIs(int index, const string value);

private:
  bool isValidJson(String root);
  void printTopic();
};

//###############################################################################
//  Topic Queue
//###############################################################################

#define TOPIC_QUEUE_MAX 10

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
    String get();
    int count= 0;
private:
    element_t* head= NULL; // we get from head
    element_t* tail= NULL; // we put to tail
};

//###############################################################################
//  API types
//###############################################################################
struct TTopic{
};
