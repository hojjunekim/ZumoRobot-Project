int trash = 0; //버퍼 초기화용 변수

void setup() {
  Serial.begin(9600); //Baud rate 9600으로 USART 시리얼 통신
  Serial.println("Please enter two numbers with one of four operations");
  Serial.println("The numbers must be between 0 and 100");
  Serial.println("You can enter up to two decimal places");
  Serial.println("Ex) 1.21+3.24");    
  Serial.println(" ");
}

void loop() { 
  float num1 = 0; //첫번째 숫자
  float num2 = 0; //두번째 숫자
  int option = 0; //사칙연산 기호
  float result = 0; //결과값
  
  while(Serial.available() == 0) {} //버퍼에 데이터 저장될 때까지 기다리기
  num1 = Serial.parseFloat(); //float형으로 입력받음
  option = Serial.read(); //연산기호는 1byte이므로 read 함수 이용
  num2 = Serial.parseFloat();
  
  if((0<num1 && 100>num1)&&(0<num2 && 100>num2)){ //입력된 숫자 범위 조건
    if(option == 42) result = num1*num2; // ’*’ 아스키코드 42
    else if(option == 43) result = num1+num2; // ’+‘ 아스키코드 43
    else if(option == 45) result = num1-num2; // ’-‘ 아스키코드 45
    else if(option == 47) result = num1/num2; // ’/’ 아스키코드 47
    
    Serial.print(num1,2); //소수점 2자리까지 출력
    Serial.print(char(option)); //아스키 코드에서 문자로 변환
    Serial.print(num2,2);
    Serial.print(char(61)); //아스키코드 문자형으로 ‘=’ 등호 출력
    Serial.println(result,2);
    Serial.println(" ");

    trash = Serial.read(); //버퍼 초기화
  }
  else{ //숫자 범위 조건 만족 안하면 오류 메시지 출력
    Serial.println("Please write the numbers between 0 and 100");
    Serial.println(" ");
    
    trash = Serial.read(); //버퍼 초기화
  }
}
