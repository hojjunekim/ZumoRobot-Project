#include <Wire.h>
#include <Pushbutton.h>
Pushbutton button(ZUMO_BUTTON);

byte SAD1 = 0b0011101;  // LSM303D Slave Adress 설정
byte SAD2 = 0b1101011;  // L3GD20H Slave Adress 설정
float WX = 0, WY = 0, WZ = 0;  // xyz angular rate [deg/s]
float MX = 0, MY = 0, MZ = 0;  // xyz magnetic [mgauss]
float AX = 0, AY = 0, AZ = 0;  // xyz acceleration [m/s^2]   
//float MagMinX = -623.84, MagMinY = -294.24, MagMinZ = -2462.24;
//float MagMaxX = 87.52, MagMaxY = 482.56, MagMaxZ = -1594.88;
float MagMinX = 32767, MagMinY = 32767, MagMinZ = 32767;  // 2^15-1
float MagMaxX = -32768, MagMaxY = -32768, MagMaxZ = -32768;  // -2^15
float WZMax = -245, WZMin = 245, WZbefore = 0;  // full scale 245dps
float EX = 0, EY = 0, EZ = 0, Emag = 0;  // East vector
float NX = 0, NY = 0, NZ = 0, Nmag = 0;  // North vector
float sfW = 0.00875;   // angular rate sensitivity [dps/digit]
float sfA = 0.061;  // +-2g Linear acceleration sensitivity [mg/LSB]
float sfM = 0.16;  // +-4gauss Magnetic sensitivity [mgauss/LSB]
float mg = 9.81/1000;  // [m/s^2]
float head = 0, angle = 0, theta = 0;
float timestart = 0;

void setup() {
  Serial.begin(9600);  // 시리얼 통신
  Wire.begin();  // I2C 통신 
  // LSM303D
  enterRegister(SAD1,0x20,0x77);  // CTRL1 ODR 200Hz, Acc all axes enabled
  enterRegister(SAD1,0x21,0x00);  // CTRL2 acceleration full scale +-2g
  enterRegister(SAD1,0x24,0x64);  // CTRL5 high resolution, ODR 6.25 Hz
  enterRegister(SAD1,0x25,0x20);  // CTRL6 magnetic full scale +-4gauss
  enterRegister(SAD1,0x26,0x00);  // CTRL7 continuous conversion mode
  // L3GD20H
  enterRegister(SAD2,0x20,0x0F); // CTRL1 ODR 12.5Hz, normal mode, all axes enabled
  enterRegister(SAD2,0x23,0x00); // CTRL4 full scale 245dps
  
  button.waitForButton();
  delay(500);
  CalibrateM();  // magnetic calibration
  Serial.print("magnetic calibration finished");
  delay(500);
  button.waitForButton();
  delay(500);
  CalibrateW();  // calibrate z-axis angular rate
  Serial.println("WZ Calibrate finished");
  delay(5000);
}


void loop() {
  MeasureA();  // AX, AY, AZ에 acceleration data 입력 [m/s^2]
  MeasureM();  // MX, MY, MZ에 magnetic data 입력 [mgauss]
  
  MX-=(MagMaxX+MagMinX)/2;  // offset 보정
  MY-=(MagMaxY+MagMinY)/2;
  MZ-=(MagMaxZ+MagMinZ)/2;
  
  GetHeading();  // calculate heading
  GetAngle();  // calculate angle(complementary filter 적용)
  //GetGyroTheta();
  
  Serial.print("Complementary filter angle: ");
  Serial.print(angle);
  Serial.print(" deg ");
  //Serial.print(theta);
  //Serial.println(" deg ");
  //Serial.print(head);
  //Serial.print(" deg");
  Serial.println(" ");
}

// GyroYaw+heading complementary filter 적용 함수
void GetAngle(void){
  timestart = millis();  // 시간 측정 시작
  MeasureW();  // WX, WY, WZ에 angular rate data 입력 [deg/s]
  WZ -= (WZMax+WZMin)/2;  // offset 보정
  delay(80);  // ODR 12.5Hz이므로 1000/ODR = 80ms
  //complementary filter
  angle = 0.95*(angle+(WZ+WZbefore)/2*(millis()-timestart)*0.001)+0.05*head;
  WZbefore = WZ;  //이전 WZ값 업데이트
}

// 2-(b)에서 사용한 gyro 센서만 이용하여 각도 구하는 함수
void GetGyroTheta(void){
  timestart = millis();  // 시간 측정 시작
  MeasureW();  // WX, WY, WZ에 angular rate data 입력 [deg/s]
  WZ -= (WZMax+WZMin)/2;  // offset 보정
  delay(80);  // ODR 12.5Hz이므로 1000/ODR = 80ms
  theta += (WZ+WZbefore)/2*0.001*(millis()-timestart);  // 사다리꼴 공식으로 적분
  WZbefore = WZ;  //이전 WZ값 업데이트
}

void GetHeading(void){
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

  head=-atan2(EX, NX)*180/PI;  // 자북에 대한 각도 heading
  return;
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

void CalibrateW(void){
  timestart = millis();  // 시간 측정 시작
  while(1){
    MeasureW();  // WX, WY, WZ에 angular rate data 입력 [deg/s]
    if((millis()-timestart)<5000){  // 5s 지나면 else문에서 return
      WZMax = max(WZMax, WZ);  // WZ 최댓값 저장
      WZMin = min(WZMin, WZ);  // WZ 최솟값 저장
      Serial.print("WZ max: ");
      Serial.print(WZMax);  //저장되고 있는 값 출력
      Serial.print(" deg/s ");
      Serial.print(" WZ min: ");
      Serial.print(WZMin);
      Serial.println(" deg/s");
      delay(50);
    }
    else{
      return;
    }
  }
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

void MeasureW(void){
  WX = (rxFromRegister(SAD2,0x28) | (rxFromRegister(SAD2,0x29)<<8))*sfW;
  WY = (rxFromRegister(SAD2,0x2A) | (rxFromRegister(SAD2,0x2B)<<8))*sfW;
  WZ = (rxFromRegister(SAD2,0x2C) | (rxFromRegister(SAD2,0x2D)<<8))*sfW;
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
