int ledNum = 8; //떨어지고 있는 LED 위치 좌표. 처음 루프는 불을 다 끄기 위해 7이 아닌 8이 들어감
int count = 0; //쌓인 LED 블록 개수

void setup() {
  for(int i = 0; i < 8; i++){
    pinMode(i, OUTPUT); //LED 핀 output 설정
  }
}

void loop() {
  for(int i = 0; i < 8; i++){
    if(i == ledNum) digitalWrite(ledNum, HIGH); //떨어지는 LED 켜주기
    else if(i > count-1) digitalWrite(i,LOW); // i가 쌓인 LED 개수보다 크면 끄기
    else digitalWrite(i,HIGH); //나머지 i는 쌓인 LED 핀이므로 모두 켜주기
  }
  
  delay(300); //300ms = 0.3sec 딜레이
  
  ledNum--; //떨어지는 LED 좌표 0.3초마다 한칸씩 내리기
  if(ledNum == count-1){ //떨어지는 LED가 쌓인 LED 블록과 겹치게 되는 조건
    ledNum = 7; //count 1이상부터는 위에서 바로 LED가 떨어져야 하므로 8이 아닌 7이 들어감
    count += 1; //쌓인 블록개수 1개 추가
  }
  if(count == 8){ //8칸 모두 LED 블록 쌓일 경우
    count = 0; //쌓인 LED 블록 초기화
    ledNum = 8; //제일 처음에는 불이 다 꺼져야 하므로 한루프 쉬기 위해 7이 아닌 8이 들어감
  }
}
