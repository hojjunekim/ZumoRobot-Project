#include <Wire.h>
#include <Pushbutton.h>
Pushbutton button(ZUMO_BUTTON);

byte SAD1 = 0b0011101;  // LSM303D Slave Adress 설정
float MX = 0, MY = 0, MZ = 0;  // xyz magnetic [mgauss]
float sfM = 0.16;  // +-4gauss Magnetic sensitivity [mgauss/LSB]
float MagMinX = 32767, MagMinY = 32767, MagMinZ = 32767;  // 2^15-1
float MagMaxX = -32768, MagMaxY = -32768, MagMaxZ = -32768;  // -2^15
int timestart=0;
float Mmag = 0;  // magnetic magnitude

void setup() {
  Serial.begin(9600);  //시리얼 통신
  Wire.begin();  // I2C 통신
  enterRegister(SAD1,0x24,0x64);  // CTRL5 high resolution, ODR 6.25 Hz
  enterRegister(SAD1,0x25,0x20);  // CTRL6 magnetic full scale +-4gauss
  enterRegister(SAD1,0x26,0x00);  // CTRL7 continuous conversion mode
  button.waitForButton();
  delay(500);
  CalibrateM();  // magnetic calibration
  Serial.println("magnetic calibration finished");
  Serial.print(MagMinX);
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
  Serial.println(' ');
  delay(3000);
}

void loop() {
  MeasureM();  // MX, MY, MZ에 magnetic data 입력 [mgauss]

  MX-=(MagMaxX+MagMinX)/2;  // offset 보정
  MY-=(MagMaxY+MagMinY)/2;
  MZ-=(MagMaxZ+MagMinZ)/2;

  Mmag = sqrt(MX*MX+MY*MY+MZ*MZ);  // total field
  
  Serial.print("MX: ");
  Serial.print(MX);
  Serial.print(" mgauss ");
  Serial.print("MY: "); 
  Serial.print(MY);
  Serial.print(" mgauss ");
  Serial.print("MZ: ");
  Serial.print(MZ);
  Serial.print(" mgauss ");
  Serial.print("M: ");
  Serial.print(Mmag);
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

void CalibrateM(void){
  timestart=millis();  // 시간 측정 시작
  while(1){
    MeasureM();
    if(millis()-timestart<20000){  // 20s 지나면 else문에서 return
      MagMinX = min(MagMinX, MX);  // magnetic 최솟값 저장
      MagMinY = min(MagMinY, MY);
      MagMinZ = min(MagMinZ, MZ);
      
      MagMaxX = max(MagMaxX, MX);  // magnetic 최댓값 저장
      MagMaxY = max(MagMaxY, MY);
      MagMaxZ = max(MagMaxZ, MZ);

      Serial.print(MagMinX);  // 측정되고 있는 값 출력
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
