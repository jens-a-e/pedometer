#include <Arduino_LSM9DS1.h>

#define DEBUG 1

float threshold = 6;
float xval[100] = {0};
float yval[100] = {0};
float zval[100] = {0};

float xavg, yavg, zavg;
int steps, flag = 0;

void setup() {
	Serial.begin(9600);
  // pinMode(LED_BUILTIN, OUTPUT);
	if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  
}

void loop() {
  track();
}

void simple_test() {

  if (IMU.accelerationAvailable()) {
  	float x, y, z;
	  IMU.readAcceleration(x, y, z);
	  double force = sqrt(x * x + y * y + z * z);
	  Serial.print("Force\t");
	  Serial.println(force);
  }

  delay(200);
}


void track() {

  int acc = 0;
  float totvect[100] = {0};
  float totave[100] = {0};

  float xaccl[100] = {0};
  float yaccl[100] = {0};
  float zaccl[100] = {0};
  for (int a = 0; a < 100; a++)
  {
  	// wait for the unit to have new meassurement
	  while (!IMU.accelerationAvailable());
    IMU.readAcceleration(xaccl[a], yaccl[a], zaccl[a]);

    totvect[a] = sqrt(((xaccl[a] - xavg) * (xaccl[a] - xavg)) + ((yaccl[a] - yavg) * (yaccl[a] - yavg)) + ((zval[a] - zavg) * (zval[a] - zavg)));
    totave[a] = (totvect[a] + totvect[a - 1]) / 2 ;
//    Serial.println("totave[a]");
    Serial.println(totave[a]);
    delay(100);
    if (totave[a] > threshold && flag == 0)
    {
      steps = steps + 1;
      flag = 1;
    }
    else if (totave[a] > threshold && flag == 1)
    {
      // Don't Count
    }
    if (totave[a] < threshold   && flag == 1)
    {
      flag = 0;
    }
    if (steps < 0) {
      steps = 0;
    }
    Serial.println('\n');
    Serial.print("steps: ");
    Serial.println(steps);
    delay(1000);
  }
  delay(1000);
}

void calibrate()
{
  
  float xaccl[100] = {0};
  float yaccl[100] = {0};
  float zaccl[100] = {0};
  float sum = 0;
  float sum1 = 0;
  float sum2 = 0;

  // Wait for the device

  for (int i = 0; i < 100; i++)
  {
    while (!IMU.accelerationAvailable());
    // wait for the unit to have new meassurement
    IMU.readAcceleration(xaccl[i], yaccl[i], zaccl[i]);
    
    sum = xaccl[i] + sum;
    sum1 = yval[i] + sum1;
    sum2 = zval[i] + sum2;
  }
  xavg = sum / 100.0;
  yavg = sum1 / 100.0;
  zavg = sum2 / 100.0;
  delay(100);

  #ifdef DEBUG
  Serial.println(xavg);
  Serial.println(yavg);
  Serial.println(zavg);
  #endif
}
