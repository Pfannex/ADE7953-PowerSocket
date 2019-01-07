#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
// ...your includes here...
#include <helper_3dmath.h>

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "washer"
#define DEVICEVERSION "1.0"

// maintainer: Bengt Giger

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);
  String fillDashboard();

private:
  int measure_water();
  int measure_acceleration();

  uint8_t mpuStatus;      // return status after each device operation (0 = success, !0 = error)
  bool dmpReady = false;  // set true if DMP init was successful
  uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
  uint16_t fifoCount;     // count of all bytes currently in FIFO
  uint8_t fifoBuffer[64]; // FIFO storage buffer

    // orientation/motion vars
  Quaternion q;           // [w, x, y, z]         quaternion container
  VectorInt16 aa;         // [x, y, z]            accel sensor measurements
  VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
  VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
  VectorFloat gravity;    // [x, y, z]            gravity vector

  int acceleration;
  int xAccelOffset = X_ACCEL_OFFSET;
  int yAccelOffset = Y_ACCEL_OFFSET;
  int zAccelOffset = Z_ACCEL_OFFSET;

  void inform();
  unsigned long lastPoll= 0;

  void calibrate();
  bool calibration_running = false;
  String calibrationState;
};
