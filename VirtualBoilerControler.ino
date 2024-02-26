#include <SHT1x.h> // 온습도센서 라이브러리
#include <Wire.h> // LCD 모듈 라이브러리
#include <LiquidCrystal.h> // LCD 모듈 라이브러리
#define dataPin 22 // 온습도 핀 [J2]
#define clockPin 23 // 온습도 핀 [J2]
SHT1x sht1x(dataPin, clockPin); // 온습도 핀 설정 [J2] 
LiquidCrystal lcd(40, 54, 41, 42, 43, 44, 45); // LCD핀 설정 [J6]
const int pwmpin1 = 8, pwmpin2 = 9; // 부저 핀 설정 [J2]
const int ledPin1=24, ledPin2=25, ledPin3=26, ledPin4=27, sw1=28, sw2=29, sw3=30, sw4=31; // [J4] LED,sw 핀
unsigned int set_Temp = 16; // 초기 설정 온도 16도

void setup() {
  pinMode(ledPin1, OUTPUT); // led 핀 설정
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(sw1, INPUT); // 스위치 핀 설정
  pinMode(sw2, INPUT);
  pinMode(sw3, INPUT);
  pinMode(sw4, INPUT);
  pinMode(pwmpin1, OUTPUT); // 부저 핀 설정
  pinMode(pwmpin2, OUTPUT);
  digitalWrite(ledPin1, HIGH); // led핀 꺼짐상태로 초기화
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin3, HIGH);
  digitalWrite(ledPin4, HIGH);
  lcd.begin(8,2); // 8,2열 LCD 시작
  lcd.clear(); // LCD 화면 초기화
}

void loop() {
  int temp_c = sht1x.readTemperatureC(); // temp_c에 온도값 저장
  
  if(set_Temp < 10) {  // 10도 이하로 온도 설정 못하게 막는 코드
    delay(100);
    set_Temp = 10;
  }
  else if(set_Temp > 40) { // 40도 이상으로 온도 설정 못하게 막는 코드
    delay(300);
    set_Temp = 40;
  }

  lcd.setCursor(0,0); // 윗줄 시작
  lcd.print("Tem:");
  lcd.print(temp_c); // 현재 온도 lcd에 출력
  lcd.write(0xDF);
  lcd.print('C');
  lcd.setCursor(0,1); // 아랫줄 시작
  lcd.print("Set:");
  lcd.print(set_Temp); // 설정 온도 lcd출력
  lcd.write(0xDF);
  lcd.print('C');
  if(digitalRead(sw1)==LOW) { // 설정온도 내리는 1번 스위치
    tone(pwmpin2, 264, 100); // 1번 스위치 누르면 "도" 출력
    set_Temp--;
    delay(100);
  }
  else if(digitalRead(sw2)==LOW) { // 설정온도 올리는 2번 스위치
    tone(pwmpin2, 297, 100); // 2번 스위치 누르면 "레" 출력
    set_Temp++;
    delay(100);
  }

  if(set_Temp>=temp_c) { // 설정온도가 현재온도보다 같거나 높으면 보일러 작동
    digitalWrite(ledPin1, LOW); // 보일러 작동 1번 led on
    digitalWrite(ledPin3, HIGH); // 보일러 작동 하면 온수전용모드 자동으로 꺼짐
    digitalWrite(ledPin4, HIGH); // 보일러 작동 하면 외출모드 자동으로 꺼짐
  }
  else {
    digitalWrite(ledPin1, HIGH); // 보일러 작동 중지 1번 led off
  }

  if(digitalRead(ledPin3)==HIGH && digitalRead(sw3)==LOW) { // 3번 스위치 누르면 온수전용모드 켜짐 3번 led on
    delay(100);
    set_Temp = temp_c-5; // 설정온도를 현재온도보다 5도 내림(온수전용모드 키면 자동으로 보일러 작동중지)
    tone(pwmpin2, 330, 100); // 3번 스위치 눌러서 온수전용모드 켰을 때, "미" 출력
    lcd.setCursor(0,0); // 윗줄에,
    lcd.print("HotWater"); // "HotWater" 메시지 출력
    lcd.setCursor(0,1); // 아랫줄에
    lcd.print("Mode ON ");  // "Mode ON " 메시지 출력
    delay(1000);
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, HIGH); // 외출모드가 켜져있으면 자동으로 꺼짐
  }
  else if(digitalRead(ledPin3)==LOW && digitalRead(sw3)==LOW) { // 3번 스위치 누르면 온수전용모드 꺼짐 3번 led off
    delay(100);
    digitalWrite(ledPin3, HIGH);
    tone(pwmpin2, 352, 100); // 3번 스위치 눌러서 온수전용모드 껐을 때, "파" 출력
    lcd.setCursor(0,0); // 윗 줄에
    lcd.print("HotWater"); // "HotWater" 메시지 출력
    lcd.setCursor(0,1); // 아랫줄에
    lcd.print("Mode OFF"); // "Mode ON " 메시지 출력
    delay(1000); // 메시지가 1초 동안 보였다가 사라짐
  }

  if(digitalRead(ledPin4)==HIGH && digitalRead(sw4)==LOW) { // 4번 스위치 누르면 외출모드 켜짐 4번 led on
    delay(100);
    set_Temp = temp_c-5; // 설정온도를 현재온도보다 5도 내림(외출모드를 키면 보일러 작동을 중단한다.)
    tone(pwmpin2, 396, 100); // 4번 스위치 눌러서 온수전용모드 껐을 때, "솔" 출력
    lcd.setCursor(0,0); // 윗 줄에
    lcd.print("OutGoing"); // "OutGoing" 메시지 출력
    lcd.setCursor(0,1); // 아랫줄에
    lcd.print("Mode ON "); // "Mode ON" 메시지 출력
    delay(1000); // 메시지가 1초 동안 보였다가 사라짐
    digitalWrite(ledPin3, HIGH); // 외출모드를 키면 온수모드도 같이 끈다.
    digitalWrite(ledPin4, LOW);
  }
  else if(digitalRead(ledPin4)==LOW && digitalRead(sw4)==LOW) { // 4번 스위치 누르면 외출모드 꺼짐 4번 led off
    delay(100);
    tone(pwmpin2, 440, 100); // 4번 스위치 눌러서 온수전용모드 껐을 때, "라" 출력
    lcd.setCursor(0,0); // 윗 줄에
    lcd.print("OutGoing"); // "OutGoing" 메시지 출력
    lcd.setCursor(0,1); // 아랫줄에
    lcd.print("Mode OFF"); // "Mode OFF" 메시지 출력
    delay(1000); // 메시지가 1초 동안 보였다가 사라짐
    digitalWrite(ledPin4, HIGH);
  }
}