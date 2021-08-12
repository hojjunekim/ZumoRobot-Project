int ledNum = 8;
int count = 0;

float Ainput = 0; //가변저항 입력값
float dtime = 0; //딜레이에 넣을 시간 변수 [ms]

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT); //가변저항 아날로그 input 핀 설정
  for(int i = 0; i < 8; i++){
    pinMode(i, OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 8; i++){
    if(i == ledNum) digitalWrite(ledNum, HIGH);
    else if(i > count-1) digitalWrite(i,LOW);
    else digitalWrite(i,HIGH);   
  }
  
  Ainput = analogRead(0); //가변저항에서 입력되는 값 저장. 범위 [0,1023]
  dtime = 100 + Ainput / 1023 * 1900; //0일 경우 100ms, 1023일 경우 2000ms
  delay(dtime); // 0.1초에서 2초 사이 딜레이
  
  ledNum--;
  if(ledNum == count-1){
    ledNum = 7;
    count += 1;
  }
  if(count == 8){
    count = 0;
    ledNum = 8;
  }
}
