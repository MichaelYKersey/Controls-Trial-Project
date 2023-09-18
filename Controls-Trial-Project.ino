#include <Servo.h>

Servo gimbalServo;

void setup() {
  Serial.begin(9600);
  Serial.println("Start Program");
  gimbalServo.attach(2);
  gimbalServo.write(90);
}

void loop() {
  int pos = 90;// TODO update servo based on MPU
  gimbalServo.write(pos);
}
