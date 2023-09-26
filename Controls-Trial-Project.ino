#include <Servo.h>
#include <Wire.h>

Servo gimbalServo;
const int kServoPort = 5;

const byte kMPUAdress = 0x68;
const byte kTempetureRegesterAdress = 0x41;
const byte kXGyroAdress = 0x43;

void setup() {
  Serial.begin(9600);// Bit send rate for Serial port
  Serial.println("Start Program");
  gimbalServo.attach(kServoPort);//make servo
  gimbalServo.write(90);// reset servo to middle
}

void loop() {
  int pos = 90;// TODO update servo based on MPU
  gimbalServo.write(pos);
  delay(100);
  Serial.println("Temp:"+ (String)(getDPMData(kTempetureRegesterAdress,2)/340.00+36.53));
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
