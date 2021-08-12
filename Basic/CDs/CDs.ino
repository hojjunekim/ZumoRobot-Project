#define D 3 
#define A A0 
#define LED 6 // analogWrite 함수가 동작하는 디지털 핀으로 설정
int Anout = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //시리얼 통신
  pinMode(D,INPUT); //조도 센서 DO. 본 코드에서는 사용안하므로 필요 없음
  pinMode(A,INPUT); //조도 센서 AO
  pinMode(LED,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Anout = analogRead(A); //조도값 읽어오기. 0~1023 범위
  Serial.print(Anout / 4); //조도값을 확인하기 위해 시리얼 모니터에 출력
  Serial.println();
  analogWrite(LED, Anout / 4); //0~255사이의 값만 허용되므로 4로 나누어주면 범위가 맞음
  delay(100); //100ms 딜레이
}
