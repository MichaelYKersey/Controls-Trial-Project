#include <Servo.h>
#include <Wire.h>

Servo gimbalServo;

void setup() {
  Serial.begin(9600);// Bit send rate for Serial port
  Serial.println("Start Program");
  gimbalServo.attach(5);
  gimbalServo.write(90);
}

void loop() {
  int pos = 90;// TODO update servo based on MPU
  gimbalServo.write(pos);
  delay(100);
  Serial.println("Temp:"+ (String)(getDPMData(0x41,2)/340.00+36.53));
}
int getDPMData(byte dataAdrees, int bytesRequested) {
  Wire.beginTransmission(0x68);
  Wire.write(dataAdrees);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,bytesRequested,true);
  int bytes = Wire.read();
  for (int i = 1; i< bytesRequested; i++){
    bytes = (bytes << 8) + Wire.read();
  }
  return bytes;
}
