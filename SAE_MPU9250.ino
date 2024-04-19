#include "MPU9250.h"

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
int status;

void setup() {
  // serial to display data
  Serial.begin(115200);
  while(!Serial) {}

  // start communication with IMU 
  status = IMU.begin();
  if (status < 0) {
    Serial.println("ERREUR");
    while(1) {}
  }
}

void loop() {
  // read the sensor
  IMU.readSensor();
  // display the data
  Serial.print("AccelX: ");
  Serial.print(IMU.getAccelX_mss(),6);
  Serial.print("	");
  Serial.print("AccelY: ");  
  Serial.print(IMU.getAccelY_mss(),6);
  Serial.print("	");
  Serial.print("AccelZ: ");  
  Serial.println(IMU.getAccelZ_mss(),6);
  
  Serial.print("GyroX: ");
  Serial.print(IMU.getGyroX_rads(),6);
  Serial.print("	");
  Serial.print("GyroY: ");  
  Serial.print(IMU.getGyroY_rads(),6);
  Serial.print("	");
  Serial.print("GyroZ: ");  
  Serial.println(IMU.getGyroZ_rads(),6);

  delay(200);
} 
