#define rightDIR 7
#define rightPWM 9
#define leftDIR 8
#define leftPWM 10

#define FORWARD 0
#define BACKWARD 1

#define trig 2
#define echo 5

#define rate (float) 17/1000 //단위 환산 값. 340*(10^2)/(10^6)/2

unsigned long duration;
float distance; //단위 환산한 거리 값. [cm]

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //시리얼 통신
  pinMode(leftDIR, OUTPUT);
  pinMode(leftPWM, OUTPUT);
  pinMode(rightDIR, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(trig, OUTPUT); //초음파 출력
  pinMode(echo, INPUT); //초음파 입력
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trig, HIGH);
  delayMicroseconds(15); // 15us 동안 초음파 출력
  digitalWrite(trig, LOW);

  duration = pulseIn(echo,HIGH); //펄스의 길이 읽어오기 [us]
  distance = rate*duration; //단위 환산된 거리 값 [cm]
  Serial.print(distance); //거리 값을 확인하기 위해 시리얼 모니터에 출력
  Serial.println(" cm"); //단위 [cm]
  
  int PWM = 15; //거리가 25.5cm보다 크면 일정하게 약한 속도로 움직이게 조절
  if (distance < 25.5) PWM = distance * 10; //analogWrite 함수가 255까지만 입력받으므로 25.5cm까지만 조건 적용
  
  digitalWrite(leftDIR, FORWARD); //왼쪽 바퀴 앞으로 이동
  analogWrite(leftPWM, PWM); //왼쪽 바퀴 PWM 출력
  digitalWrite(rightDIR, FORWARD); //오른쪽 바퀴 앞으로 이동
  analogWrite(rightPWM, PWM); //오른쪽 바퀴 PWM 출력
  delay(60); // 60ms 딜레이
}
