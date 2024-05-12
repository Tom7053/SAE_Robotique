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
    if (x <= 0.5 or y <= 0.5) {
      angle = 0;
    } else {
      angle = atan2(x, y) * (180 / PI);
    }
    Serial.println(angle);
    delay(500);
  }
}
