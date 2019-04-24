#include "framework/Core/Clock.h"
#include "framework/Utils/SysUtils.h"

//###############################################################################
//  NTP clock
//###############################################################################
//-------------------------------------------------------------------------------
//  NTP clock public
//-------------------------------------------------------------------------------

Clock::Clock(TopicQueue &topicQueue) : topicQueue(topicQueue), tz(CET, CEST) {
  uptimeLo = 0;
  uptimeHi = 0;
}

Clock::~Clock() { stop(); }
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
//  uptime routines
//...............................................................................

void Clock::updateUptime() {
  unsigned long ms = millis();
  if (ms < uptimeLo) {
    // counter wraparound
    uptimeHi++;
  }
  uptimeLo = ms;
}

unsigned long long Clock::uptimeMillis() {
  return uptimeLo + uptimeHi * 4294967296;
}

//...............................................................................
//  now
//...............................................................................

time_t Clock::now() {

  if (ntpClient == nullptr) {
    return (time_t)(uptimeMillis() / 1000);
  } else {
    return tz.toLocal(ntpClient->getEpochTime());
  }
}

unsigned long long Clock::nowMillis() {

  if (ntpClient == nullptr) {
    return uptimeMillis();
  } else {
    unsigned long long t = ntpClient->getEpochMillis();
    unsigned long long ts = t / 1000;
    return t + (tz.toLocal(ts) - ts) * 1000;
  }
}

//...............................................................................
//  update clock
//...............................................................................

// this function needs to be called periodically
void Clock::handle() {

  updateUptime();
  time_t t;
  if (ntpClient != nullptr) {
    // this polls the NTP server only if NTP_UPDATE_INTERVAL has elapsed
    ntpClient->update();
  }
  t = now();
  if (t != lastTime) {
    lastTime = t;
#if defined(CLOCKTICK_MINUTE)
    struct tm *tp = localtime(&t);
    if (!tp->tm_sec)
      topicQueue.put("~/event/clock/time " + SysUtils::strDate(t) + " " +
                     SysUtils::strTimeHHMM(t));
#else
    topicQueue.put("~/event/clock/time " + SysUtils::strDateTime(t));
#endif
  }
}

void Clock::forceUpdate() {

  if (ntpClient != nullptr) {
    ntpClient->forceUpdate();
  }
}

//...............................................................................
//  time differential in seconds between local time and UTC
//...............................................................................

long Clock::timezone(time_t t) { return t - tz.toUTC(t); }

//...............................................................................
//  root() get root string as JSON
//...............................................................................

String Clock::root() {

  time_t t = now();
  return "{\"date\":\"" + SysUtils::strDate(t) + "\"," + "\"time\":\"" +
         SysUtils::strTime(t) + "\"," + "\"dateTime\":\"" +
         SysUtils::strDateTime(t) + +"\"," + "\"uptime\":\"" +
         SysUtils::uptimeStr(uptimeMillis()) + "\"," + "\"timezone\":\"" +
         String(timezone(t), DEC) + "\"}";
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
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  Clock GET
//...............................................................................

/*
~/set
  └─clock
      ├─root
      ├─time
      ├─date
      ├─dateTime
      ├─uptime
      └─timezone
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
    /*} else if (topic.itemIs(3, "dateTime_ms")) {
      return SysUtils::strDateTime_ms(now());*/
  } else if (topic.itemIs(3, "uptime")) {
    return SysUtils::uptimeStr(uptimeMillis());
  } else if (topic.itemIs(3, "timezone")) {
    return String(timezone(now()), DEC);
  } else {
    return TOPIC_NO;
  }
}
