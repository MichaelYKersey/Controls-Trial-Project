#include <Servo.h>
#include <Wire.h>

Servo gimbalServo;
const int kServoPort = 5;

int prevTime = -1;
double rollDeg = 0;
int i=0;

const byte kMPUAdress = 0x68;
const byte kTempetureRegesterAdress = 0x41;

const byte kXGyroOutAdress = 0x43;
const byte kGyroConfigAdress = 0x1B;
const byte kGyroConfig = 0b00000000;
const byte kPowerManagement1Adress = 0x6B;
const double kGyroBitsToDegPerSec = 1/131.0; //131 is the LSB/deg/sec for The Gyro

void setup() {
  Serial.begin(9600);// Bit send rate for Serial port
  Serial.println("Start Program");
  gimbalServo.attach(kServoPort);//make servo
  gimbalServo.write(90);// reset servo to middle

  //wake up MPU
  Wire.beginTransmission(kMPUAdress);
  Wire.write(kPowerManagement1Adress);
  Wire.write(0x00);
  Wire.endTransmission(true);

  //config MPU
  Wire.beginTransmission(kMPUAdress);
  Wire.write(kGyroConfigAdress);
  Wire.write(kGyroConfig);
  Wire.endTransmission(true);
}

void loop() {
  int pos = 90;// TODO update servo based on MPU
  gimbalServo.write(pos);
  updateRoll();
  if (i==10000) {
    Serial.println("Temp:"+ (String)(getDPMData(kTempetureRegesterAdress,2)/340.00+36.53));
    Serial.println("XGyro:"+ (String)(getDPMData(kXGyroOutAdress,2) * kGyroBitsToDegPerSec));
    Serial.println("Roll:"+ (String)(rollDeg));
    i=0;
  }
  i++;
}

void updateRoll() {
  // if (prevTime == -1) {
  //   prevTime = millis();
  //   return;
  // }
  int time = millis();
  double timeDeltaSec = (time - prevTime) / 1000.0; // diffrence in time in seconds
  double rollVel = getDPMData(kXGyroOutAdress,2) * kGyroBitsToDegPerSec;
  rollDeg += rollVel * timeDeltaSec;
  prevTime = time;
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
