#include "MPU9250.h"

#define PI 3.14159

double angle;

MPU9250 IMU(Wire, 0x68);
int status;

int main(void) {
  Serial.begin(115200);
  while (!Serial) {}

  status = IMU.begin();
  if (status < 0) {
    Serial.println("ERREUR");
    while (1) {}
  }


  while (1) {

    IMU.readSensor();
    double x = IMU.getMagX_uT();
    double y = IMU.getMagY_uT();
    Serial.println(x);
    Serial.println(y);
    angle = mpu.getYaw();
    Serial.println(angle);
    delay(500);
  }
}
