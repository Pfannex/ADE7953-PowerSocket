#include "framework/Core/ESP.h"
#include "framework/Utils/SysUtils.h"
#include "framework/Core/OmniESPUpdater.h"

//###############################################################################
//  ESP
//###############################################################################
ESP_Tools::ESP_Tools(LOGGING &logging) : logging(logging) {}

//-------------------------------------------------------------------------------
//  ESP public
//-------------------------------------------------------------------------------

void ESP_Tools::start() {

  char txt[128];
  sprintf(txt, "chip id:         %08X", ESP.getChipId());
  logging.debug(String(txt));
  sprintf(txt, "core version:    %s", ESP.getCoreVersion().c_str());
  logging.debug(String(txt));
  sprintf(txt, "boot version:    %d", ESP.getBootVersion());
  logging.debug(String(txt));
  sprintf(txt, "sdk version:     %s", ESP.getSdkVersion());
  logging.debug(String(txt));
  sprintf(txt, "cpu frequency:   %d MHz", ESP.getCpuFreqMHz());
  logging.debug(String(txt));
  sprintf(txt, "boot mode:       %d", ESP.getBootMode());
  logging.debug(String(txt));

  checkFlash();
  //sprintf(deviceName, "%06x", chipId());

}

//...............................................................................
//  check FlashConfiguration
//...............................................................................
void ESP_Tools::checkFlash() {
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();

  logging.info("checking flash memory");

  char txt[128];
  // Device= HHLL, Manufacturer= MM
  sprintf(txt, "flash chip id:   %08X", ESP.getFlashChipId()); // 00LLHHMM
  logging.debug(String(txt));
  sprintf(txt, "flash real size: %u", realSize);
  logging.debug(String(txt));
  sprintf(txt, "flash ide  size: %u", ideSize);
  logging.debug(String(txt));
  sprintf(txt, "flash ide speed: %u", ESP.getFlashChipSpeed());
  logging.debug(String(txt));
  sprintf(txt, "flash ide mode:  %s",
          (ideMode == FM_QIO
               ? "QIO"
               : ideMode == FM_QOUT
                     ? "QOUT"
                     : ideMode == FM_DIO
                           ? "DIO"
                           : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  logging.debug(String(txt));

  if (ideSize != realSize) {
    logging.error("flash chip configuration wrong");
  } else {
    logging.info("flash chip configuration ok");
  }
}

//...............................................................................
//  ESP reboot
//...............................................................................
void ESP_Tools::reboot() {
  // due to an issue in the silicon chip,
  // this works only after one hardware reset
  // has occured after flashing
  ESP.restart();
}

//...............................................................................
//  get free memSize
//...............................................................................
uint32_t ESP_Tools::freeHeapSize() { // long?
  return ESP.getFreeHeap();
}

int ESP_Tools::freeStackSize() {
  /*
  register uint32_t *sp asm("a1");
  return 4 * (sp - g_cont.stack);
  */
  return -1;
}

int ESP_Tools::minFreeStackSize() {
/*return cont_get_free_stack(&g_cont);*/
return -1; }

int ESP_Tools::stackCorrupted() {
  /*
  return cont_check(&g_cont);
  */
  return false;
}

//...............................................................................
//  get Chip ID
//...............................................................................
long ESP_Tools::chipId() { return ESP.getChipId(); }

String ESP_Tools::genericName() { return String(deviceName); }

//...............................................................................
//  update
//...............................................................................

bool ESP_Tools::updateRequested() {
  return updateRequest;
}

String ESP_Tools::update() {

  updateRequest= false; // avoid infinite loops right from the beginning

  OmniESPUpdater U(logging);

  if(U.doUpdate(DEFAULTTARBALL, setDeviceDefaults)) {
    return TOPIC_OK;
    // reboot(); this is done in the Controller
  } else {
    return U.getLastError();
  }

}

//...............................................................................
//  DEBUG MEM
//...............................................................................
void ESP_Tools::debugMem() {
  char msg[200];
  sprintf(msg, "free heap: %d, free stack: %d (min: %d, corrupt: %d)",
          freeHeapSize(), freeStackSize(), minFreeStackSize(),
          stackCorrupted());
  logging.debug(msg);
}

void ESP_Tools::debugMem_start() {
  stackStart = freeStackSize();
  debugMem();
}

void ESP_Tools::debugMem_stop() {
  debugMem();
  char msg[200];
  sprintf(msg, "StackStart: %d, StackStop: %d, diff %d", stackStart,
          freeStackSize(), freeStackSize() - stackStart);
  logging.debug(msg);
}

//...............................................................................
//  ESP_Tools SET
//...............................................................................

/*

~/set/
  └──esp
     └─restart
*/
String ESP_Tools::set(Topic &topic) {
  if (topic.itemIs(3, "restart")) {
    reboot();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "update")) {
    // we need to set the updateRequest flag here only because we cannot
    // run the update from an asynchronous call like from the webserver,
    // the core would panic when calling yield()
    setDeviceDefaults= topic.argIs(0, "defaults");
    updateRequest= true;
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  ESP_Tools GET
//...............................................................................

/*

~/get/
  └──esp
     ├─chipId
     └─freeHeapSize
*/

String ESP_Tools::get(Topic &topic) {
  if (topic.itemIs(3, "chipId")) {
    return String(chipId());
  } else if (topic.itemIs(3, "freeHeapSize")) {
    return String(freeHeapSize());
  } else {
    return TOPIC_NO;
  }
}
