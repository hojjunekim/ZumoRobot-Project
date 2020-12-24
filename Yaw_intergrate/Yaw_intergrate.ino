#include <Wire.h>
#include <Pushbutton.h>
Pushbutton button(ZUMO_BUTTON);

byte SAD2 = 0b1101011;  // L3GD20H Slave Adress 설정
float WX = 0, WY = 0, WZ = 0;  // xyz angular rate
float WZMax = -245, WZMin = 245, WZbefore = 0;  // full scale 245dps
float sfW = 0.00875; // angular rate sensitivity [dps/digit]                    
float theta  = 0;
float timestart = 0;

void setup() {
  Serial.begin(9600);  // 시리얼 통신
  Wire.begin();  // I2C 통신
  enterRegister(SAD2,0x20,0x0F); // CTRL1 ODR 12.5Hz, normal mode, all axes enabled
  enterRegister(SAD2,0x23,0x00); // CTRL4 full scale 245dps
  button.waitForButton();
  delay(500);
  CalibrateW();  // calibrate z-axis angular rate
  Serial.println("WZ Calibrate finished");
  delay(5000);
}


void loop() {
  GetTheta();  // start calculating theta
  Serial.print(theta);
  Serial.print(" deg");
  Serial.println(" ");
}

void GetTheta(void){
  timestart = millis();  // 시간 측정 시작
  MeasureW();  // WX, WY, WZ에 angular rate data 입력 [deg/s]
  WZ -= (WZMax+WZMin)/2;  // offset 보정
  delay(80);  // ODR 12.5Hz이므로 1000/ODR = 80ms
  theta += (WZ+WZbefore)/2*0.001*(millis()-timestart);  // 사다리꼴 공식으로 적분
  WZbefore = WZ;  //이전 WZ값 업데이트
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
