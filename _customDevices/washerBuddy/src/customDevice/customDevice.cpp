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


int customDevice::measure_acceleration() {
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
    return sqrt(aaWorld.x*aaWorld.x + aaWorld.y*aaWorld.y + aaWorld.z*aaWorld.z);
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
  if (!calibration_running) {
    acceleration = measure_acceleration();
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
  if (topic.itemIs(3, "calibrate")) {
    calibrate();
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
  } else if (topic.itemIs(3, "calibrationState")) {
    return calibrationState;
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

String customDevice::fillDashboard() {
  topicQueue.put("~/event/device/water", measure_water());
  topicQueue.put("~/event/device/xAccelOffset", xAccelOffset);
  topicQueue.put("~/event/device/yAccelOffset", yAccelOffset);
  topicQueue.put("~/event/device/zAccelOffset", zAccelOffset);
  topicQueue.put("~/event/device/calibrationState " + calibrationState);

  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

//...............................................................................
// calibration run - determine acceleration offset parameters
// taken from IMU_Zero from the MPU6050 examples,
// 2016-10-19 by Robert R. Fenichel (bob@fenichel.net)
//...............................................................................

const int iAx = 0;
const int iAy = 1;
const int iAz = 2;
const int iGx = 3;
const int iGy = 4;
const int iGz = 5;

const int usDelay = 3150;   // empirical, to hold sampling to 200 Hz
const int NFast =  1000;    // the bigger, the better (but slower)
const int NSlow = 10000;    // ..
const int LinesBetweenHeaders = 5;
      int LowValue[6];
      int HighValue[6];
      int Smoothed[6];
      int LowOffset[6];
      int HighOffset[6];
      int Target[6];
      int LinesOut;
      int N;

void SetAveraging(int NewN)
  { N = NewN;
   } // SetAveraging

void GetSmoothed()
  { int16_t RawValue[6];
    int i;
    long Sums[6];
    for (i = iAx; i <= iGz; i++)
      { Sums[i] = 0; }
//    unsigned long Start = micros();

    for (i = 1; i <= N; i++)
      { // get sums
        mpu.getMotion6(&RawValue[iAx], &RawValue[iAy], &RawValue[iAz],
                             &RawValue[iGx], &RawValue[iGy], &RawValue[iGz]);
        delayMicroseconds(usDelay);
        for (int j = iAx; j <= iGz; j++)
          Sums[j] = Sums[j] + RawValue[j];
      } // get sums
//    unsigned long usForN = micros() - Start;
//    Serial.print(" reading at ");
//    Serial.print(1000000/((usForN+N/2)/N));
//    Serial.println(" Hz");
    for (i = iAx; i <= iGz; i++)
      { Smoothed[i] = (Sums[i] + N/2) / N ; }
  } // GetSmoothed

void SetOffsets(int TheOffsets[6])
  { mpu.setXAccelOffset(TheOffsets [iAx]);
    mpu.setYAccelOffset(TheOffsets [iAy]);
    mpu.setZAccelOffset(TheOffsets [iAz]);
    mpu.setXGyroOffset (TheOffsets [iGx]);
    mpu.setYGyroOffset (TheOffsets [iGy]);
    mpu.setZGyroOffset (TheOffsets [iGz]);
  } // SetOffsets

void PullBracketsIn()
  { boolean AllBracketsNarrow;
    boolean StillWorking;
    int NewOffset[6];

    AllBracketsNarrow = false;
    StillWorking = true;
    while (StillWorking)
      { StillWorking = false;
        if (AllBracketsNarrow && (N == NFast))
          { SetAveraging(NSlow); }
        else
          { AllBracketsNarrow = true; }// tentative
        for (int i = iAx; i <= iGz; i++)
          { if (HighOffset[i] <= (LowOffset[i]+1))
              { NewOffset[i] = LowOffset[i]; }
            else
              { // binary search
                StillWorking = true;
                NewOffset[i] = (LowOffset[i] + HighOffset[i]) / 2;
                if (HighOffset[i] > (LowOffset[i] + 10))
                  { AllBracketsNarrow = false; }
              } // binary search
          }
        SetOffsets(NewOffset);
        GetSmoothed();
        for (int i = iAx; i <= iGz; i++)
          { // closing in
            if (Smoothed[i] > Target[i])
              { // use lower half
                HighOffset[i] = NewOffset[i];
                HighValue[i] = Smoothed[i];
              } // use lower half
            else
              { // use upper half
                LowOffset[i] = NewOffset[i];
                LowValue[i] = Smoothed[i];
              } // use upper half
          } // closing in
      } // still working

  } // PullBracketsIn

void PullBracketsOut()
  { boolean Done = false;
    int NextLowOffset[6];
    int NextHighOffset[6];

    while (!Done)
      { Done = true;
        SetOffsets(LowOffset);
        GetSmoothed();
        for (int i = iAx; i <= iGz; i++)
          { // got low values
            LowValue[i] = Smoothed[i];
            if (LowValue[i] >= Target[i])
              { Done = false;
                NextLowOffset[i] = LowOffset[i] - 1000;
              }
            else
              { NextLowOffset[i] = LowOffset[i]; }
          } // got low values

        SetOffsets(HighOffset);
        GetSmoothed();
        for (int i = iAx; i <= iGz; i++)
          { // got high values
            HighValue[i] = Smoothed[i];
            if (HighValue[i] <= Target[i])
              { Done = false;
                NextHighOffset[i] = HighOffset[i] + 1000;
              }
            else
              { NextHighOffset[i] = HighOffset[i]; }
          } // got high values
        for (int i = iAx; i <= iGz; i++)
          { LowOffset[i] = NextLowOffset[i];   // had to wait until ShowProgress done
            HighOffset[i] = NextHighOffset[i]; // ..
          }
     } // keep going
  } // PullBracketsOut



void customDevice::calibrate() {
  if (mpuStatus != 0) return;

  calibrationState = String("Running...");
  topicQueue.put("~/event/device/calibrationState " + calibrationState);
  calibration_running = true; // lock sensor exclusively

  for (int i = iAx; i <= iGz; i++)
    { // set targets and initial guesses
      Target[i] = 0; // must fix for ZAccel
      HighOffset[i] = 0;
      LowOffset[i] = 0;
    } // set targets and initial guesses
  Target[iAz] = 16384;
  SetAveraging(NFast);

  PullBracketsOut();
  PullBracketsIn();

  xAccelOffset = (HighOffset[iAx]+LowOffset[iAx])/2;
  yAccelOffset = (HighOffset[iAz]+LowOffset[iAy])/2;
  zAccelOffset = (HighOffset[iAz]+LowOffset[iAz])/2;

  ffs.deviceCFG.writeItem("xAccelOffset", String(xAccelOffset));
  ffs.deviceCFG.writeItem("yAccelOffset", String(yAccelOffset));
  ffs.deviceCFG.writeItem("zAccelOffset", String(zAccelOffset));
  topicQueue.put("~/event/device/xAccelOffset");
  topicQueue.put("~/event/device/yAccelOffset");
  topicQueue.put("~/event/device/zAccelOffset");

  logging.info("set new acceleration offsets, X: " + String(xAccelOffset) +
               ", Y: " + String(yAccelOffset) + String(zAccelOffset));

  calibration_running = false; // unlock sensor
  calibrationState = String("Idle");
  topicQueue.put("~/event/device/calibrationState " + calibrationState);
}
