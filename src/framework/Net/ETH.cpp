#include "framework/Net/ETH.h"

//###############################################################################
//  Ethernet
//###############################################################################

/*
int _builtin_led_state = 1;

void ICACHE_RAM_ATTR eth_loop(void) {
  digitalWrite(LED_BUILTIN, _builtin_led_state);
  _builtin_led_state = 1 - _builtin_led_state;
}

bool schedule_function_us(const std::function<bool(void)> &fn,
                          uint32_t repeat_ms = 0);
*/
//###############################################################################

ETH::ETH(LOGGING &logging, FFS &ffs)
    : eth(SPI, CSPIN), logging(logging), ffs(ffs) {}

void ETH::start() {

  Serial.println("starting ethernet...");
  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
  //SPI.setBitOrder(MSBFIRST);
  //SPI.setDataMode(SPI_MODE0);

  eth.setDefault(); // use ethernet for default route
  present = eth.begin();
  if (!present) {
    Serial.println("no ethernet hardware present");
    //return;
  } else {
    Serial.print("connecting ethernet");
    while (!eth.connected()) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println();
    Serial.print("ethernet ip address: ");
    Serial.println(eth.localIP());
  }





/*
  pinMode(LED_BUILTIN, OUTPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);

  present = eth.begin();

  if (!present) {
    logging.info("no ethernet hardware present");
    return;
  } else {
#define MAXTRIES 10
    int i = 0;
    int c;
    logging.info("connecting ethernet...");
    while (i < MAXTRIES) {
      c = eth.connected();
      if (c)
        break;
      delay(1000);
      i++;
    }
    if (c) {
      logging.info("ethernet ip address: " + eth.localIP().toString());
      logging.info("ethernet gw address: " + eth.gatewayIP().toString());
      logging.info("ethernet netmask   : " + eth.subnetMask().toString());
      eth.setDefault();
    } else {
      logging.error("could not connect ethernet");
    }
  }
*/
}

void ETH::stop() {
//  if (present) {
 //   eth.end();
//  }
}

void ETH::handle() {
   // yield();
}

int ETH::isPresent() {
    //return present;
}

