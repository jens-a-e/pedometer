/**
 * Pedometer - Work in Progress
 *
 * A step tracker using the algorithm of the Open Source C-Step-Counter by Anna
 * Brondin & Marcus Nordstrom at Malmö University -
 * https://github.com/MarcusNordstrom/C-Step-Counter
 *
 * Copyright (C) 2021  jens alexander ewald <jens@poetic.systems>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * ------
 *
 * This project has received funding from the European Union’s Horizon 2020
 * research and innovation programme under the Marie Skłodowska-Curie grant
 * agreement No. 813508.
 */

#include <Arduino_LSM9DS1.h>

extern "C" {
  #include "C-Step-Counter/include/StepCountingAlgo.h"
}

//#define DEBUG

float threshold = 6;
float xval[100] = {0};
float yval[100] = {0};
float zval[100] = {0};

float xavg, yavg, zavg;
int steps, flag = 0;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  IMU.setAccelFS(0);
  IMU.setAccelODR(3);           //
  IMU.setAccelOffset(0, 0, 0);  //   uncalibrated
  IMU.setAccelSlope (1, 1, 1);  //   uncalibrated

  Serial.print("Accelerometer Full Scale = ±");
  Serial.print(IMU.getAccelFS());
  Serial.println ("g");

  initAlgo();
}

void loop() {
  track();
}

void simple_test() {

  if (IMU.accelerationAvailable()) {
    float x, y, z;
    IMU.readRawAccel(x, y, z);
    double force = sqrt(x * x + y * y + z * z);
    Serial.print("Force\t");
    Serial.println(force);
  }

  delay(200);
}

/**
 * Retuns the amount of steps detected
 */
int track() {
  static int stepsBefore = 0;
  float x = 0;
  float y = 0;
  float z = 0;

  // wait for the unit to have new meassurement
  while (!IMU.accelerationAvailable());
  IMU.readRawAccel(x, y, z);
  processSample(millis(), int32_t(x), int32_t(y), int32_t(z));

  #ifdef DEBUG
    double force = sqrt(x * x + y * y + z * z);
    Serial.print("Force\t");
    Serial.println(force);
    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(z);
  #endif
  
  int steps = getSteps();
  if (stepsBefore > 0 && steps != stepsBefore) {
    Serial.println(steps);
    delay(200);
    stepsBefore = steps;
    return steps - stepsBefore;
  }
  return 0;
}

//void calibrate()
//{
//
//  float xaccl[100] = {0};
//  float yaccl[100] = {0};
//  float zaccl[100] = {0};
//  float sum = 0;
//  float sum1 = 0;
//  float sum2 = 0;
//
//  // Wait for the device
//
//  for (int i = 0; i < 100; i++)
//  {
//    while (!IMU.accelerationAvailable());
//    // wait for the unit to have new meassurement
//    IMU.readAcceleration(xaccl[i], yaccl[i], zaccl[i]);
//
//    sum = xaccl[i] + sum;
//    sum1 = yval[i] + sum1;
//    sum2 = zval[i] + sum2;
//  }
//  xavg = sum / 100.0;
//  yavg = sum1 / 100.0;
//  zavg = sum2 / 100.0;
//  delay(100);
//
//#ifdef DEBUG
//  Serial.println(xavg);
//  Serial.println(yavg);
//  Serial.println(zavg);
//#endif
//}
