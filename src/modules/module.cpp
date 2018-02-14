#include "modules/module.h"

//===============================================================================
//  module
//===============================================================================
module::module(string name, LOGGING &logging, TopicQueue &topicQueue)
    : name(name), logging(logging), topicQueue(topicQueue) {}

//-------------------------------------------------------------------------------
//  module public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void module::start() {
  logging.info("starting module "+String(name));
  nameItemCount= 1;
  for(string p= name; *p; nameItemCount+= (*p++=='/'));
  //logging.debug(String(nameItemCount)+" item(s) in "+name);
  if(nameItemCount!=1)
    logging.error(String(nameItemCount)+" item(s) in "+String(name));
}

//...............................................................................
// handle
//...............................................................................
void module::handle() {
  // intentionally left blank
}

//-------------------------------------------------------------------------------
// isModule
//-------------------------------------------------------------------------------

bool module::isForModule(Topic &topic) {
  /*
  ~/get|set|event     0 1
    └─device          2
        └─(name)      3 .. 3+(nameItemCount-1)

  total: 3+nameItemCount
  */
  if(topic.getItemCount() < 3+nameItemCount)
    return false;
  if(!topic.itemIs(2, "device"))
    return false;
  if (topic.itemIs(3, name)) {
    return true;
  } else {
    return false;
  }

}

//-------------------------------------------------------------------------------
// isItem
//-------------------------------------------------------------------------------

bool module::isItem(Topic &topic, string item) {
  /*
  ~/get|set|event     0 1
    └─device          2
        └─<name>      3 .. 3+(nameItemCount-1)
            └─<item>  4+(nameItemCount-1)

  total: 4+nameItemCount
  */
  if(topic.getItemCount() != 4+nameItemCount)
    return false;
  return topic.itemIs(3+nameItemCount, item);

}
