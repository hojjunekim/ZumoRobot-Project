#include <ZumoShield.h>

ZumoReflectanceSensorArray reflectanceSensors; //QTR 센서
ZumoMotors motors; //속도 설정 편하게 해주기 위한 함수
Pushbutton button(ZUMO_BUTTON); //zumo 버튼 설정

const int maxSpeed = 140; //최고 속력 설정

void setup() {
  reflectanceSensors.init(); //QTR 센서 initialize

  button.waitForButton(); //버튼 눌려질 때까지 대기

  pinMode(13, OUTPUT); //내장 LED
  digitalWrite(13, HIGH); //버튼 눌려지면 내장 LED 켜짐

  //검은선 근처에 놔두어 보정이 잘되도록 한다.
  delay(1000); //1초 기다리고 calibrate 시작
  for(int i = 0; i < 80; i++){
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70)) //회전 전환 반복
      motors.setSpeeds(-200, 200); //반시계방향 회전
    else
      motors.setSpeeds(200, -200); //시계방향 회전
    reflectanceSensors.calibrate(); //센서 calibrate

    delay(20); //i값 하나당 0.02초 딜레이
  }

  motors.setSpeeds(0,0); //zumo robot 정지

  digitalWrite(13, LOW); //내장 LED 불 끄기

  button.waitForButton(); //루프문으로 이동하기 전에 버튼 눌려질 때까지 대기
}


void loop() {
  unsigned int sensors[6];
  reflectanceSensors.read(sensors); //센서 6개 값 받아오기
  
  long sum = 0;
  long sum2 = 0;

  //센서는 양 끝 두개만 사용
  sum2 = (long)sensors[5]*5000; //가중치 적용된 0, 5센서 합
  sum = sensors[0]+sensors[5]; //0, 5센서 합
  int result = sum2/sum; //가중치 합과 센서 합의 비율

  int error = result-2500; //평균 2500에서 벗어난 값

  if(sensors[0]<200&&sensors[5]<200) //양 끝 센서 신호가 약할 경우(경로 밖)
    motors.setSpeeds(maxSpeed,maxSpeed); //직진

  //에러가 양수일수록 오른쪽 센서 sensors[5]이 어두운 경로 위인 것
  //에러가 음수일수록 왼쪽 센서 sensors[0]이 어두운 경로 위인 것
  //오른쪽이 경로 위이면 왼쪽 바퀴가 빨라지고 오른쪽 바퀴가 느려져야함
  //왼쪽이 경로 위이면 왼쪽 바퀴가 느려지고 오른쪽 바퀴가 빨라져야함
  int leftSpeed = maxSpeed + error; //왼쪽 바퀴는 에러 더해줌
  int rightSpeed = maxSpeed - error; //오른쪽 바퀴는 에러 빼줌

  if (leftSpeed < 0) //속도가 음수면
    leftSpeed = 0; //0으로 설정
  if (rightSpeed < 0)
    rightSpeed = 0;
  if (leftSpeed > maxSpeed) //속도가 최고속력보다 크면
    leftSpeed = maxSpeed; //최고속력으로 설정
  if (rightSpeed > maxSpeed)
    rightSpeed = maxSpeed;
  
  motors.setSpeeds(leftSpeed,rightSpeed); //zumo robot에 속도 입력
}
