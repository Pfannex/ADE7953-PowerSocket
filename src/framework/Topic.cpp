#include "Arduino.h"
#include "framework/Utils/Debug.h"
#include "framework/Topic.h"

//###############################################################################
//  data exchange Topic
//###############################################################################
Topic::Topic(string topicsArgs) {
  // D("Topic: begin constructor(string)");
  //DF("Topic::Topic(string topicsArgs) begin");
  initTopic(topicsArgs);
  //DF("Topic::Topic(string topicsArgs) end");
  // D("Topic: end constructor(string)");
}

Topic::Topic(String &topicsArgs) {
  // D("Topic: begin constructor(String&)");
  initTopic(topicsArgs.c_str());
  // D("Topic: end constructor(String&)");
}

Topic::Topic(string topics, string args) { dissectTopic(topics, args); }

Topic::Topic(string topics, int value) {
  char args[20];
  sprintf(args, "%d", value);
  dissectTopic(topics, args);
}

Topic::~Topic() {
  //DF("Topic::~Topic() begin");
  item.clear();
  arg.clear();
  //DF("Topic::~Topic() end");
}

//-------------------------------------------------------------------------------
//  Topic public
//-------------------------------------------------------------------------------
//...............................................................................
//  Topic strings
//...............................................................................
String Topic::topic_asString() {
  //Di("Topic: begin topic_asString", item.getCount());
  String str = String(item.string[0]);
  for (int i = 1; i < item.getCount(); i++) {
    str += "/" + String(item.string[i]);
  }
  return str;
}
String Topic::arg_asString() {
  String str = "";
  if (arg.getCount() > 0)
    str = String(arg.string[0]);
  for (int i = 1; i < arg.getCount(); i++) {
    str += "," + String(arg.string[i]);
  }
  return str;
}
String Topic::asString() { return topic_asString() + " " + arg_asString(); }

//...............................................................................
//  set item
//...............................................................................

void Topic::setItem(unsigned int index, const string topicName) {
  if(index< item.getCount()) {
    item.replace(index, strdup(topicName));
  }
}
//...............................................................................
//  delete TopicItem in return String
//...............................................................................
String Topic::modifyTopic(int index) {
  String str = "";

  for (int i = 0; i < item.getCount(); i++) {
    if (i != index) {
      str += String(item.string[i]);
      if (i != item.getCount() - 1) {
        str += "/";
      }
    }
  }
  return str;
}

//...............................................................................
//  compare topic.item by index with string
//...............................................................................
bool Topic::itemIs(int index, string topicName) {
  // D("Topic: begin itemIs");
  // Di(topicName, index);
  if (index < item.getCount())
    return !strcmp(item.string[index], topicName);
  else
    return false;
  // D("Topic: end itemIs");
}

//...............................................................................
//  compare topic.arg by index with string
//...............................................................................
bool Topic::argIs(int index, const string value) {
  if (index < arg.getCount())
    return !strcmp(arg.string[index], value);
  else
    return false;
}

//...............................................................................
//  get number of items
//...............................................................................

int Topic::getItemCount() {
  return item.getCount();
}
//...............................................................................
//  get argument referenced by index
//...............................................................................

string Topic::getItem(int index) {
  if (index < item.getCount())
    return item.string[index];
  else
    return NULL;
}

long Topic::getItemAsLong(int index) {
  if (index < item.getCount())
    // The  strtol() function converts the initial part of the string
    // to a long integer value according to the given base, which must
    // be between 2 and 36 inclusive, or be the special value 0.
    // The string may begin with an arbitrary amount of white space
    // (as determined by isspace(3)) followed by a single optional
    // '+'  or  '-' sign. If base is zero or 16, the string may then
    // include a "0x" or "0X" prefix, and the number will be read in
    // base 16; otherwise, a zero base is taken as 10 (decimal) unless
    //  the next character is '0', in which case it is taken as 8 (octal).
    return strtol(item.string[index], NULL, 0);
  else
    return 0;
}
//...............................................................................
//  get number of arguments
//...............................................................................
int Topic::getArgCount() {
    return arg.getCount();
}

//...............................................................................
//  get argument referenced by index
//...............................................................................

string Topic::getArg(int index) {
  if (index < arg.getCount())
    return arg.string[index];
  else
    return NULL;
}

long Topic::getArgAsLong(int index) {
  if (index < arg.getCount())
    // The  strtol() function converts the initial part of the string
    // to a long integer value according to the given base, which must
    // be between 2 and 36 inclusive, or be the special value 0.
    // The string may begin with an arbitrary amount of white space
    // (as determined by isspace(3)) followed by a single optional
    // '+'  or  '-' sign. If base is zero or 16, the string may then
    // include a "0x" or "0X" prefix, and the number will be read in
    // base 16; otherwise, a zero base is taken as 10 (decimal) unless
    //  the next character is '0', in which case it is taken as 8 (octal).
    return strtol(arg.string[index], NULL, 0);
  else
    return 0;
}


//-------------------------------------------------------------------------------
//  Topic private
//-------------------------------------------------------------------------------

//...............................................................................
//  init from string
//...............................................................................
void Topic::initTopic(string topicsArgs) {

  // D("Topic: begin initTopic");
  // D(topicsArgs);
  char *topics, *args;
  char *tmp = strdup(topicsArgs);
  topics = strtok(tmp, " ");
  args = strtok(NULL, "\0");
  dissectTopic(topics, args);
  free(tmp);
  // D("Topic: end initTopic");
}

//...............................................................................
//  break into tokens
//...............................................................................

// str can be an empty string but it cannot be NULL
int Topic::tokenize(strings &tokens, string str, string delimiters) {
  // D("tokenize...");
  tokens.clear();
  char *token;
  char *s = strdup(str);
  // D(s);
  token = strtok(s, delimiters);
  // Dl;
  while (token != NULL) {
    // D(token);
    if (*token) {
      tokens.append(strdup(token));
      // Di(tokens.string[tokens.getCount() - 1], tokens.getCount());
    }
    token = strtok(NULL, delimiters);
    // Dl;
  }
  free(s);
  return tokens.getCount();
}

//...............................................................................
//  check for valid JSON-String
//...............................................................................
bool Topic::isValidJson(String root) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.parseObject(root);
  return json.success();
}

//...............................................................................
//  dissect Topic
//...............................................................................
// topics and args can be empty or NULL
void Topic::dissectTopic(string topics, string args) {

Serial.println("Topic::dissectTopic: " + String(topics) + " " + String(args));

   //D("Topic: begin dissectTopic");
   //Dl;
  // items
  item.clear();
  if (topics != NULL)
    tokenize(item, topics, "/");
   //Di("items=", item.getCount());
  // args
  arg.clear();
  if (args != NULL) {
    if (isValidJson(String(args))) {
      arg.append(strdup(args));
    } else {
      tokenize(arg, args, ",");
      // Di("args=", arg.getCount());
    }
  }
   //D("Topic: end dissectTopic");
}

//###############################################################################
//  Topic Queue
//###############################################################################

TopicQueue::TopicQueue() {}

void TopicQueue::clear() {
  while (count)
    get();
}

void TopicQueue::put(element_t *e) {
  //DF("TopicQueue::put(element_t) begin");
  if (count < TOPIC_QUEUE_MAX) {
    // prepend to tail
    e->next = tail;
    if (count) {
      // if the queue has elements, backlink previous tail to new element
      tail->prev = e;
    } else {
      // else the new element is also made the head
      head = e;
    }
    // the new element is made the tail
    tail = e;
    // increase count
    count++;
    // Serial.println("queue: put done");
  } else {
    // queue is full, throw the element away
    delete e;
  }
  //DF("TopicQueue::put(element_t) end");
}

void TopicQueue::put(String &topicsArgs) {
  // Serial.println("queue: put "+topicsArgs);
  // create a new element
  element_t *e = new element_t;
  e->topicsArgs = strdup(topicsArgs.c_str());
  put(e);
}

void TopicQueue::put(const char *topicsArgs) {
  // create a new element
  element_t *e = new element_t;
  e->topicsArgs = strdup(topicsArgs);
  put(e);
}

void TopicQueue::put(const char *topics, int arg) {
  char topicsArgs[128];
  sprintf(topicsArgs, "%s %d", topics, arg);
  put(topicsArgs);
}

String TopicQueue::get() {

  // D("queue: get begin");
  //DF("TopicQueue::get() begin");
  if (count) {
    element_t *e = head;
    String topicsArgs = String(e->topicsArgs);
    free(e->topicsArgs);
    head = head->prev;
    delete e;
    count--;
    // D("queue: get done");
    //DF("TopicQueue::get() end");
    return topicsArgs;
  } else {
    // D("queue: get error");
    return String(
        ""); // if you get here then you have made an programming error
  }
}
