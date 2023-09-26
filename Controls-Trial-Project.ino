#include <Servo.h>
#include <Wire.h>

Servo gimbalServo;
const int kServoPort = 5;

const byte kMPUAdress = 0x68;
const byte kTempetureRegesterAdress = 0x41;

const byte kXGyroOutAdress = 0x43;
const byte kGyroConfigAdress = 0x1B;
const byte kGyroConfig = 0 << 3;
const double kGyroBitsToDegPerSec = 2/131.0; //131 is the LSB/deg/sec for The Gyro

void setup() {
  Serial.begin(9600);// Bit send rate for Serial port
  Serial.println("Start Program");
  gimbalServo.attach(kServoPort);//make servo
  gimbalServo.write(90);// reset servo to middle

  //config MPU
  Wire.write(kMPUAdress);
  Wire.write(kGyroConfigAdress);
  Wire.write(kGyroConfig);
  Wire.endTransmission(true);
}

void loop() {
  int pos = 90;// TODO update servo based on MPU
  gimbalServo.write(pos);
  delay(1000);
  // Serial.println("Temp:"+ (String)(getDPMData(kTempetureRegesterAdress,2)/340.00+36.53));
  Serial.println("XGyro:"+ (String)(getDPMData(kXGyroOutAdress,2) * kGyroBitsToDegPerSec));
}

int getDPMData(byte dataAdrees, int bytesRequested) {
  Wire.beginTransmission(kMPUAdress);
  Wire.write(dataAdrees);
  Wire.endTransmission(false);
  Wire.requestFrom(kMPUAdress,bytesRequested,true);
  int bytes = Wire.read();
  for (int i = 1; i< bytesRequested; i++){
    bytes = (bytes << 8) + Wire.read();
  }
  return bytes;
}
