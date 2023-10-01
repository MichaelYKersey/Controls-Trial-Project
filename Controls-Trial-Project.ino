#include <Servo.h>
#include <Wire.h>

Servo gimbalServo;
const int kServoPort = 5;

unsigned long prevTime = -1;
double rollDeg = 0;
int iteration=0;

const byte kMPUAdress = 0x68;
const byte kTempetureRegesterAdress = 0x41;

const byte kXGyroOutAdress = 0x43;
const byte kGyroConfigAdress = 0x1B;
const byte kGyroConfig = 0b00000000;
const byte kPowerManagement1Adress = 0x6B;
const double kGyroBitsToDegPerSec = (1<<0)/131.0; //131 is the LSB/deg/sec for The Gyro
double drift;

void setup() {
  Serial.begin(9600);// Bit send rate for Serial port
  Serial.println("Start Program");
  gimbalServo.attach(kServoPort);//make servo
  gimbalServo.write(90);// reset servo to middle

  //reset MPU
  Wire.beginTransmission(kMPUAdress);
  Wire.write(kPowerManagement1Adress);
  Wire.write(0b10000000);
  Wire.endTransmission(true);

  delay(10000);

  //wake up MPU
  Wire.beginTransmission(kMPUAdress);
  Wire.write(kPowerManagement1Adress);
  Wire.write(0b00000000);
  Wire.endTransmission(true);

  delay(100);

  //config MPU
  Wire.beginTransmission(kMPUAdress);
  Wire.write(kGyroConfigAdress);
  Wire.write(kGyroConfig);
  Wire.endTransmission(true);

  delay(100);

  Serial.println("Starting Calibration");
  setDrift();
  Serial.println("Calibration Finished");
  delay(3000);

  prevTime = millis(); 
}

void loop() {
  updateRoll();
  int pos = (int)rollDeg + 90;// TODO update servo based on MPU
  if (pos < 0) pos=0;
  if (pos > 180) pos=180;
  //pos=90;
  gimbalServo.write(pos);
  if (iteration==100) {
    // Serial.println("Temp:"+ (String)(getDPMData(kTempetureRegesterAdress,2)/340.00+36.53));
    // Serial.println("XGyro:"+ (String)(getDPMData(kXGyroOutAdress,2) * kGyroBitsToDegPerSec-drift));
    Serial.println("Roll:"+ (String)(rollDeg));
    iteration=0;
  }
  iteration++;
}

void updateRoll() {
  // if (prevTime == -1) {
  //   prevTime = millis();
  //   return;
  // }
  unsigned long time = millis();
  double timeDeltaSec = (time - prevTime) / 1000.0; // diffrence in time in seconds
  prevTime = time;

  int messurement = getDPMData(kXGyroOutAdress,2);
  double rollVel = messurement * kGyroBitsToDegPerSec - drift;
  rollDeg += rollVel * timeDeltaSec;
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

void setDrift(){
  // int start = millis();
  int count =0;
  double rollVelTotal = 0;
  // while (start + 10000 > millis()) {
  for (int i=0; i < 1000; i++) {
    delay(3);
    rollVelTotal += getDPMData(kXGyroOutAdress,2) * kGyroBitsToDegPerSec;
    count++;
  }
  drift = rollVelTotal/count;
  Serial.println("Drift:"+ (String)(drift));// + "," + (String)(count));
  
}
