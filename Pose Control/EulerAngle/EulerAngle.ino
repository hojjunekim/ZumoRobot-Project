#include <Wire.h>
#include <Pushbutton.h>
Pushbutton button(ZUMO_BUTTON);

byte SAD1 = 0b0011101;  // LSM303D Slave Adress 설정
float MX = 0, MY = 0, MZ = 0;  // xyz magnetic [mgauss]
float AX = 0, AY = 0, AZ = 0;  // xyz acceleration [m/s^2]     
float sfA = 0.061;  // +-2g Linear acceleration sensitivity [mg/LSB]
float sfM = 0.16;  // +-4gauss Magnetic sensitivity [mgauss/LSB]
float mg = 9.81/1000;  // [m/s^2]
float psi = 0, theta = 0, phi  = 0;  // yaw  pitch roll
float MagMinX = 32767, MagMinY = 32767, MagMinZ = 32767;  // 2^15-1
float MagMaxX = -32768, MagMaxY = -32768, MagMaxZ = -32768;  // -2^15
float EX = 0, EY = 0, EZ = 0, Emag = 0;  // East vector
float NX = 0, NY = 0, NZ = 0, Nmag = 0;  // North vector
float timestart = 0;

void setup() {
  Serial.begin(9600);  // 시리얼 통신
  Wire.begin();  // I2C 통신 
  enterRegister(SAD1,0x20,0x77);  // CTRL1 ODR 200Hz, Acc all axes enabled
  enterRegister(SAD1,0x21,0x00);  // CTRL2 acceleration full scale +-2g
  enterRegister(SAD1,0x24,0x64);  // CTRL5 high resolution, ODR 6.25 Hz
  enterRegister(SAD1,0x25,0x20);  // CTRL6 magnetic full scale +-4gauss
  enterRegister(SAD1,0x26,0x00);  // CTRL7 continuous conversion mode
  button.waitForButton();
  delay(500);
  CalibrateM();  // magnetic calibration
  Serial.print("magnetic calibration finished");
  delay(3000);
}


void loop() {
  MeasureA();  // AX, AY, AZ에 acceleration data 입력 [m/s^2]
  MeasureM();  // MX, MY, MZ에 magnetic data 입력 [mgauss]
  MX-=(MagMaxX+MagMinX)/2;  // offset 보정
  MY-=(MagMaxY+MagMinY)/2;
  MZ-=(MagMaxZ+MagMinZ)/2;
  
  phi = atan2(AY, AZ)*180/PI;  // roll
  theta = atan2(-AX, sqrt(AY*AY+AZ*AZ))*180/PI;  // pitch
  GetYaw();  // yaw(psi)

  Serial.print("phi = ");
  Serial.print(phi);
  Serial.print(" theta = ");
  Serial.print(theta);
  Serial.print(" psi = ");
  Serial.println(psi);
  delay(200);
}


void CalibrateM(void){
  timestart=millis();  // 시간 측정 시작
  while(1){
    MeasureM();
    if(millis()-timestart<20000){  // 20s 지나면 else문에서 return
      MagMinX = min(MagMinX, MX);  // magnetic 최솟값 저장
      MagMinY = min(MagMinY, MY);
      MagMinZ = min(MagMinZ, MZ);
      
      MagMaxX = max(MagMaxX, MX);  // magnetic 최솟값 저장
      MagMaxY = max(MagMaxY, MY);
      MagMaxZ = max(MagMaxZ, MZ);
      
      Serial.print(MagMinX);  // 저장되고 있는 값 출력
      Serial.print(' ');
      Serial.print(MagMinY);
      Serial.print(' ');
      Serial.print(MagMinZ);
      Serial.print(' ');
      Serial.print(MagMaxX);
      Serial.print(' ');
      Serial.print(MagMaxY);
      Serial.print(' ');
      Serial.println(MagMaxZ);
      delay(50);
    }
    else{
      return;
    }
  }
}

void GetYaw(void){
  EX=MY*AZ-MZ*AY;  // E = M X A (cross곱)
  EY=MZ*AX-MX*AZ;
  EZ=MX*AY-MY*AX;

  Emag=sqrt(EX*EX+EY*EY+EZ*EZ);
  EX/=Emag;  // E normalize
  EY/=Emag;
  EZ/=Emag;

  NX=AY*EZ-AZ*EY;  // N = A X E (cross곱)
  NY=AZ*EX-AX*EZ;
  NZ=AX*EY-AY*EX;

  Nmag=sqrt(NX*NX+NY*NY+NZ*NZ);
  NX/=Nmag;  // N normalize
  NY/=Nmag;
  NZ/=Nmag;

  psi=atan2(EX, NX)*180/PI;  // 자북에 대한 각도 heading
  return;
}

void MeasureA(void){
  AX = (rxFromRegister(SAD1,0x28) | (rxFromRegister(SAD1,0x29)<<8))*sfA*mg;
  AY = (rxFromRegister(SAD1,0x2A) | (rxFromRegister(SAD1,0x2B)<<8))*sfA*mg;
  AZ = (rxFromRegister(SAD1,0x2C) | (rxFromRegister(SAD1,0x2D)<<8))*sfA*mg;
  // 상 하위 바이트 합치고 scale factor, mg 곱해주기
  return;
}

void MeasureM(void){
  MX = (rxFromRegister(SAD1,0x08) | (rxFromRegister(SAD1,0x09)<<8))*sfM;
  MY = (rxFromRegister(SAD1,0x0A) | (rxFromRegister(SAD1,0x0B)<<8))*sfM;
  MZ = (rxFromRegister(SAD1,0x0C) | (rxFromRegister(SAD1,0x0D)<<8))*sfM;
  // 상 하위 바이트 합치고 scale factor 곱해주기
  return;
}

void enterRegister(byte Add, byte Regi_add, byte input) {
  Wire.beginTransmission(Add);  
  Wire.write(Regi_add);
  Wire.write(input);  
  Wire.endTransmission();  
}

byte rxFromRegister(byte Add, byte Regi_add) {
  Wire.beginTransmission(Add); 
  Wire.write(Regi_add); 
  Wire.endTransmission(); 
  Wire.requestFrom(Add, 1);  
  byte output = Wire.read();
  return output;
}#include <Wire.h>
#include <Pushbutton.h>
Pushbutton button(ZUMO_BUTTON);

byte SAD1 = 0b0011101;  // LSM303D Slave Adress 설정
float MX = 0, MY = 0, MZ = 0;  // xyz magnetic [mgauss]
float AX = 0, AY = 0, AZ = 0;  // xyz acceleration [m/s^2]     
float sfA = 0.061;  // +-2g Linear acceleration sensitivity [mg/LSB]
float sfM = 0.16;  // +-4gauss Magnetic sensitivity [mgauss/LSB]
float mg = 9.81/1000;  // [m/s^2]
float psi = 0, theta = 0, phi  = 0;  // yaw  pitch roll
float MagMinX = 32767, MagMinY = 32767, MagMinZ = 32767;  // 2^15-1
float MagMaxX = -32768, MagMaxY = -32768, MagMaxZ = -32768;  // -2^15
float EX = 0, EY = 0, EZ = 0, Emag = 0;  // East vector
float NX = 0, NY = 0, NZ = 0, Nmag = 0;  // North vector
float timestart = 0;

void setup() {
  Serial.begin(9600);  // 시리얼 통신
  Wire.begin();  // I2C 통신 
  enterRegister(SAD1,0x20,0x77);  // CTRL1 ODR 200Hz, Acc all axes enabled
  enterRegister(SAD1,0x21,0x00);  // CTRL2 acceleration full scale +-2g
  enterRegister(SAD1,0x24,0x64);  // CTRL5 high resolution, ODR 6.25 Hz
  enterRegister(SAD1,0x25,0x20);  // CTRL6 magnetic full scale +-4gauss
  enterRegister(SAD1,0x26,0x00);  // CTRL7 continuous conversion mode
  button.waitForButton();
  delay(500);
  CalibrateM();  // magnetic calibration
  Serial.print("magnetic calibration finished");
  delay(3000);
}


void loop() {
  MeasureA();  // AX, AY, AZ에 acceleration data 입력 [m/s^2]
  MeasureM();  // MX, MY, MZ에 magnetic data 입력 [mgauss]
  MX-=(MagMaxX+MagMinX)/2;  // offset 보정
  MY-=(MagMaxY+MagMinY)/2;
  MZ-=(MagMaxZ+MagMinZ)/2;
  
  phi = atan2(AY, AZ)*180/PI;  // roll
  theta = atan2(-AX, sqrt(AY*AY+AZ*AZ))*180/PI;  // pitch
  GetYaw();  // yaw(psi)

  Serial.print("phi = ");
  Serial.print(phi);
  Serial.print(" theta = ");
  Serial.print(theta);
  Serial.print(" psi = ");
  Serial.println(psi);
  delay(200);
}


void CalibrateM(void){
  timestart=millis();  // 시간 측정 시작
  while(1){
    MeasureM();
    if(millis()-timestart<20000){  // 20s 지나면 else문에서 return
      MagMinX = min(MagMinX, MX);  // magnetic 최솟값 저장
      MagMinY = min(MagMinY, MY);
      MagMinZ = min(MagMinZ, MZ);
      
      MagMaxX = max(MagMaxX, MX);  // magnetic 최솟값 저장
      MagMaxY = max(MagMaxY, MY);
      MagMaxZ = max(MagMaxZ, MZ);
      
      Serial.print(MagMinX);  // 저장되고 있는 값 출력
      Serial.print(' ');
      Serial.print(MagMinY);
      Serial.print(' ');
      Serial.print(MagMinZ);
      Serial.print(' ');
      Serial.print(MagMaxX);
      Serial.print(' ');
      Serial.print(MagMaxY);
      Serial.print(' ');
      Serial.println(MagMaxZ);
      delay(50);
    }
    else{
      return;
    }
  }
}

void GetYaw(void){
  EX=MY*AZ-MZ*AY;  // E = M X A (cross곱)
  EY=MZ*AX-MX*AZ;
  EZ=MX*AY-MY*AX;

  Emag=sqrt(EX*EX+EY*EY+EZ*EZ);
  EX/=Emag;  // E normalize
  EY/=Emag;
  EZ/=Emag;

  NX=AY*EZ-AZ*EY;  // N = A X E (cross곱)
  NY=AZ*EX-AX*EZ;
  NZ=AX*EY-AY*EX;

  Nmag=sqrt(NX*NX+NY*NY+NZ*NZ);
  NX/=Nmag;  // N normalize
  NY/=Nmag;
  NZ/=Nmag;

  psi=-atan2(EX, NX)*180/PI;  // 지표면 수직축에 대해 회전한 각도 heading
  return;
}

void MeasureA(void){
  AX = (rxFromRegister(SAD1,0x28) | (rxFromRegister(SAD1,0x29)<<8))*sfA*mg;
  AY = (rxFromRegister(SAD1,0x2A) | (rxFromRegister(SAD1,0x2B)<<8))*sfA*mg;
  AZ = (rxFromRegister(SAD1,0x2C) | (rxFromRegister(SAD1,0x2D)<<8))*sfA*mg;
  // 상 하위 바이트 합치고 scale factor, mg 곱해주기
  return;
}

void MeasureM(void){
  MX = (rxFromRegister(SAD1,0x08) | (rxFromRegister(SAD1,0x09)<<8))*sfM;
  MY = (rxFromRegister(SAD1,0x0A) | (rxFromRegister(SAD1,0x0B)<<8))*sfM;
  MZ = (rxFromRegister(SAD1,0x0C) | (rxFromRegister(SAD1,0x0D)<<8))*sfM;
  // 상 하위 바이트 합치고 scale factor 곱해주기
  return;
}

void enterRegister(byte Add, byte Regi_add, byte input) {
  Wire.beginTransmission(Add);  
  Wire.write(Regi_add);
  Wire.write(input);  
  Wire.endTransmission();  
}

byte rxFromRegister(byte Add, byte Regi_add) {
  Wire.beginTransmission(Add); 
  Wire.write(Regi_add); 
  Wire.endTransmission(); 
  Wire.requestFrom(Add, 1);  
  byte output = Wire.read();
  return output;
}
