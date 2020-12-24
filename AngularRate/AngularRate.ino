#include <Wire.h>

byte SAD2 = 0b1101011;  // L3GD20H Slave Adress 설정
float WX = 0, WY = 0, WZ = 0;  // xyz angular rate
float sfW = 0.00875; // angular rate sensitivity [dps/digit]          

void setup() {
  Serial.begin(9600);  //시리얼 통신
  Wire.begin();  // I2C 통신
  enterRegister(SAD2,0x20,0x0F); // CTRL1 ODR 12.5Hz, normal mode, all axes enabled
  enterRegister(SAD2,0x23,0x00); // CTRL4 full scale 245dps
}

void loop() {
  MeasureW();  // WX, WY, WZ에 angular rate 입력 [deg/s]
  Serial.print("WX: ");
  Serial.print(WX);
  Serial.print(" deg/s");
  Serial.print(" WY: ");
  Serial.print(WY);
  Serial.print(" deg/s");
  Serial.print(" WZ: ");
  Serial.print(WZ);
  Serial.print(" deg/s");
  Serial.println(" ");
  delay(100);
}


void MeasureW(void){
  WX = (rxFromRegister(SAD2,0x28) | (rxFromRegister(SAD2,0x29)<<8))*sfW;
  WY = (rxFromRegister(SAD2,0x2A) | (rxFromRegister(SAD2,0x2B)<<8))*sfW;
  WZ = (rxFromRegister(SAD2,0x2C) | (rxFromRegister(SAD2,0x2D)<<8))*sfW;
  // 상 하위 바이트 합치고 scale factor 곱해주기
  return;
}

void enterRegister(byte Add, byte Regi_add, byte input) {
  Wire.beginTransmission(Add);      // 데이터 전송 시작
  Wire.write(Regi_add);             // CTRL1 레지스터 아래의 데이터 쓰기
  Wire.write(input);                // 200Hz로 센서 구동, X,Y,Z축 모두 작동
  Wire.endTransmission();           // 데이터 전송 끝
}

byte rxFromRegister(byte Add, byte Regi_add) {
  Wire.beginTransmission(Add);      // 요청 전송 시작
  Wire.write(Regi_add);             // 레지스터 주소값 전송
  Wire.endTransmission();           // 요청 전송 끝
  Wire.requestFrom(Add, 1);         // 데이터 수신 요청. 1번만 요청
  byte output = Wire.read();
  return output;
}
