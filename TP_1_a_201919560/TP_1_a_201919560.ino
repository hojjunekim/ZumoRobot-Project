#include <Wire.h>

byte SAD1 = 0b0011101;  // LSM303D Slave Adress 설정
float MX = 0, MY = 0, MZ = 0;  // xyz magnetic [mgauss]
float sfM = 0.16;  // +-4gauss Magnetic sensitivity [mgauss/LSB]

void setup() {
  Serial.begin(9600);  //시리얼 통신
  Wire.begin();  // I2C 통신
  enterRegister(SAD1,0x24,0x64);  // CTRL5 high resolution, ODR 6.25 Hz
  enterRegister(SAD1,0x25,0x20);  // CTRL6 magnetic full scale +-4gauss
  enterRegister(SAD1,0x26,0x00);  // CTRL7 continuous conversion mode
}

void loop() {
  MeasureM();  // MX, MY, MZ에 magnetic data 입력 [mgauss]
  
  Serial.print("MX: ");
  Serial.print(MX);
  Serial.print(" mgauss ");
  Serial.print(" MY: ");
  Serial.print(MY);
  Serial.print(" mgauss ");
  Serial.print(" MZ: ");
  Serial.print(MZ);
  Serial.println(" mgauss");
  delay(200);
}

void MeasureM(void){
  MX = (rxFromRegister(SAD1,0x08) | (rxFromRegister(SAD1,0x09)<<8))*sfM;
  MY = (rxFromRegister(SAD1,0x0A) | (rxFromRegister(SAD1,0x0B)<<8))*sfM;
  MZ = (rxFromRegister(SAD1,0x0C) | (rxFromRegister(SAD1,0x0D)<<8))*sfM;
  // 상 하위 바이트 합치고 scale factor 곱해주기
  return;
}


void enterRegister(byte Add, byte Regi_add, byte input) {
  Wire.beginTransmission(Add);  // 데이터 전송 시작
  Wire.write(Regi_add);  // 레지스터 주소값 전송
  Wire.write(input);  // 레지스터에 데이터 입력
  Wire.endTransmission();  // 데이터 전송 끝
}

byte rxFromRegister(byte Add, byte Regi_add) {
  Wire.beginTransmission(Add);  // 요청 전송 시작
  Wire.write(Regi_add);  // 레지스터 주소값 전송
  Wire.endTransmission();   // 요청 전송 끝
  Wire.requestFrom(Add, 1);  // 데이터 수신 요청
  byte output = Wire.read();
  return output;
}
