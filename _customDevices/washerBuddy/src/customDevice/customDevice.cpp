#include "customDevice.h"
#include <Arduino.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>

//===============================================================================
//  Device
//===============================================================================

MPU6050 mpu;

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs) {

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  Device::start(); // mandatory

  // ... your code here ...
  //configItem = ffs.deviceCFG.readItem("configItem").toInt();
  //logging.info("configItem is "+String(configItem));

  // Water detector
  pinMode(PIN_WATERSENSOR, INPUT);

  // MPU6050 accelerometer
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  mpu.initialize();
  if (mpu.testConnection()) {
    logging.info("MPU6050 connection successful");
  } else {
    logging.info("MPU6050 connection failed");
  }
  mpuStatus = mpu.dmpInitialize();
  // supply your own offsets here, scaled for min sensitivity
  // see IMU_Zero example code for calibration
  mpu.setXAccelOffset(ffs.deviceCFG.readItem("xAccelOffset").toInt());
  mpu.setYAccelOffset(ffs.deviceCFG.readItem("yAccelOffset").toInt());
  mpu.setZAccelOffset(ffs.deviceCFG.readItem("zAccelOffset").toInt());

  // make sure it worked (returns 0 if so)
  if (mpuStatus == 0) {
    // turn on the DMP, now that it's ready
    logging.info("Enabling DMP...");
    mpu.setDMPEnabled(true);
    //logging.info("Enabling interrupt detection (Arduino external interrupt ");
    //logging.info(")...");
    //attachInterrupt(digitalPinToInterrupt(ACCELERATION_INTERRUPT), dmpDataReady, RISING);
    //mpuIntStatus = mpu.getIntStatus();
    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    //logging.info("DMP ready! Waiting for first interrupt...");
    dmpReady = true;
    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    logging.error("DMP Initialization failed (code ");
    logging.error(String(mpuStatus));
    logging.error(")");
  }

  logging.info("device running");
}

//...............................................................................
// measure
//...............................................................................

int customDevice::measure_water() {
  if (digitalRead(PIN_WATERSENSOR) == LOW) {
    return 1;
  } else {
    return 0;
  }
}

void customDevice::inform() {
  topicQueue.put("~/event/device/water", measure_water());
  topicQueue.put("~/event/device/acceleration", acceleration);
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {
  mpu.resetFIFO();
  fifoCount = mpu.getFIFOCount();
  // wait for correct available data length, should be a VERY short wait
  while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

  // read a packet from FIFO
  if (fifoCount >= packetSize) {
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    //fifoCount -= packetSize;

    // display initial world-frame acceleration, adjusted to remove gravity
    // and rotated based on known orientation from quaternion
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    acceleration = sqrt(aaWorld.x*aaWorld.x + aaWorld.y*aaWorld.y + (aaWorld.z)*(aaWorld.z));
  }

  unsigned long now = millis();
  if (now - lastPoll >= 3000) {
    lastPoll = now;
    inform();
  }
}

//...............................................................................
//  Device set
//...............................................................................

String customDevice::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─yourItem         (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/yourItem
    return TOPIC_NO;
  if (topic.itemIs(3, "yourItem")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  Device get
//...............................................................................

String customDevice::get(Topic &topic) {
  /*
  ~/get
  └─device             (level 2)
    └─sensor1          (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/sensor1
    return TOPIC_NO;
  if (topic.itemIs(3, "water")) {
    return String(measure_water());
  } else if (topic.itemIs(3, "acceleration")) {
    return String(acceleration);
  } else if (topic.itemIs(3, "xAccelOffset")) {
    return String(xAccelOffset);
  } else if (topic.itemIs(3, "yAccelOffset")) {
    return String(yAccelOffset);
  } else if (topic.itemIs(3, "zAccelOffset")) {
    return String(zAccelOffset);
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// event handler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {

  // central business logic
}
