#include "Clock.h"
#include "SysUtils.h"

//###############################################################################
//  NTP clock
//###############################################################################
//-------------------------------------------------------------------------------
//  NTP clock public
//-------------------------------------------------------------------------------

Clock::Clock(TopicQueue &topicQueue): topicQueue(topicQueue) {}

Clock::~Clock() {
  stop();
}
//...............................................................................
//  start
//...............................................................................
void Clock::start() {
  stop();

  // this function actually starts nothing
}

void Clock::start(const char *poolServerName, int timeOffset,
                  int updateInterval) {

  stop();
  ntpClient = new NTPClient(ntpUDP, poolServerName, timeOffset, updateInterval);
  ntpClient->begin();
  ntpClient->update();
}

//...............................................................................
//  stop ntp
//...............................................................................

void Clock::stop() {

  if (ntpClient != nullptr) {
    ntpClient->end();
    delete ntpClient;
    ntpClient = nullptr;
  }
  lastTime = 0;
}

//...............................................................................
//  now
//...............................................................................

time_t Clock::now() {

  if (ntpClient == nullptr) {
    return millis() / 1000;
  } else {
    return ntpClient->getEpochTime();
  }
}

//...............................................................................
//  update clock
//...............................................................................

void Clock::adjustTimeOffset() {
  if (ntpClient == nullptr) return;
  // change between summer time (DST) and winter time
  time_t t = now();
  // Sommerzeit = letzter Sonntag im März von 2h -> 3h
  if (month(t) == 3 and day(t) >= 25 and weekday(t) == 7 and hour(t) == 2)
    ntpClient->setTimeOffset(SUMMER_TIME);
  // Winterzeit = letzter Sonntag im Oktober von 3h -> 2h
  if (month(t) == 10 and day(t) >= 25 and weekday(t) == 7 and hour(t) == 3)
    ntpClient->setTimeOffset(WINTER_TIME);
}

// this function needs to be called periodically
void Clock::handle() {

  unsigned long t;
  if (ntpClient != nullptr) {
    // this polls the NTP server only if NTP_UPDATE_INTERVAL has elapsed
    ntpClient->update();
  }
  t= now();
  if(t != lastTime) {
    lastTime = t;
    adjustTimeOffset();
    // using SysUtils::strDateTime() makes the firmware crash after 6 minutes
    topicQueue.put("~/event/clock/time "+SysUtils::strDateTime(t));
    //topicQueue.put("~/event/clock/time", t);
  }
}

void Clock::forceUpdate() {

  if (ntpClient != nullptr) {
    ntpClient->forceUpdate();
    adjustTimeOffset();
  }
}

//...............................................................................
//  root() get root string as JSON
//...............................................................................

String Clock::root() {

  time_t t = now();
  return "{\"date\":\"" + SysUtils::strDate(t) + "\"," + "\"time\":\"" +
         SysUtils::strTime(t) + "\"," + "\"time_ms\":\"" +
         SysUtils::strTime_ms(t) + "\"," + "\"dateTime\":\"" +
         SysUtils::strDateTime(t) + "\"," + "\"dateTime_ms\":\"" +
         SysUtils::strDateTime(t) + "\"}";
}

//...............................................................................
//  Clock SET
//...............................................................................

String Clock::set(Topic &topic) {
  /*
  ~/set
    └─clock
        └─forceNTPUpdate
  */

  if (topic.itemIs(3, "forceNTPUpdate")) {
    forceUpdate();
    return TOPIC_OK;
  } else { return TOPIC_NO; }
}

//...............................................................................
//  Clock GET
//...............................................................................

/*
~/set
  └─clock
      ├─root
      ├─Time
      ├─Date
      ├─DateTime
      └─DateTime_ms
*/
String Clock::get(Topic &topic) {
    if (topic.itemIs(3, "root")) {
      return root();
    } else if (topic.itemIs(3, "time")) {
      return SysUtils::strTime(now());
    } else if (topic.itemIs(3, "date")) {
      return SysUtils::strDate(now());
    } else if (topic.itemIs(3, "dateTime")) {
      return SysUtils::strDateTime(now());
    } else if (topic.itemIs(3, "dateTime_ms")) {
      return SysUtils::strDateTime_ms(now());
    } else {
      return TOPIC_NO;
    }
}
