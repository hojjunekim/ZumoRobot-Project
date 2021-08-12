volatile boolean blink = LOW; //인터럽트 발생할 때마다 반대로 바꿔줄 변수
#define line 2 //인터럽트 쓸 수 있는 디지털 핀 2, 3중에 선택
#define LED 13 //내장 LED


void setup() {
  //Serial.begin(9600);
  pinMode(line, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(line), detect, CHANGE); //모드 FALLING RISING CHANGE 중 선택 가능
}

void loop() {
  if(blink){ //blink == HIGH 일 때 진입. LED 깜빡이는 코드
    digitalWrite(LED, HIGH);
    delay(300);
    digitalWrite(LED, LOW);
    delay(300);
  }
  else{ //blink == LOW
    digitalWrite(LED, LOW); //LED 꺼짐
  }
  //int digout = digitalRead(line);
  //Serial.println(digout); //검출안되면 1, 검출되면 0이 나타남을 확인
  //delay(300);
}

void detect(){ //인터럽트 발생시 진입
  blink = !blink; //blink 반대로 변환
}
