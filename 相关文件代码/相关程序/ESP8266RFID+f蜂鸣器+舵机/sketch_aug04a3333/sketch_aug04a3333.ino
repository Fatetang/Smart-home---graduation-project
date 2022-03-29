/*
Many thanks to nikxha from the ESP8266 forum
*/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h> //调用库文件

/* wiring the MFRC522 to ESP8266 (ESP-12)
RST     = GPIO5
SDA(SS) = GPIO4 
MOSI    = GPIO13
MISO    = GPIO12
SCK     = GPIO14
GND     = GND
3.3V    = 3.3V
*/

#define RST_PIN  5  // RST-PIN für RC522 - RFID - SPI - Modul GPIO5 
#define SS_PIN  4  // SDA-PIN für RC522 - RFID - SPI - Modul GPIO4 
#define myservo_PIN 16 //宏定义舵机控制引脚
#define BUZZER 2

const char *ssid =  "TP-LINK";     // change according to your Network - cannot be longer than 32 characters!
const char *pass =  "TANG1808494756"; // change according to your Network

char * UID[2]={"4F D7 83 6F","97 36 7E B4"};

Servo myservo;  // 创建一个伺服电机对象 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

int angle1 = 90;  //原始角度值 
int angle2 = 180;  //旋转角度值

void setup() {
  
  pinMode(BUZZER,OUTPUT);
  digitalWrite(BUZZER,HIGH);
  Serial.begin(9600);    // Initialize serial communications   
  delay(250);
  myservo.attach(myservo_PIN);  
  Serial.println(F("Booting...."));
  
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  
  WiFi.begin(ssid, pass);
  
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 10)) {
    retries++;
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected"));
  }
  Serial.println(F("Ready!"));
  Serial.println(F("======================================================")); 
  Serial.println(F("Scan for Card and print UID:"));
}

void loop() { 
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //在串行监视器上显示UID
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == UID[1]) //change here the UID of the card/cards that you want to give access
  {
    Serial.println("授权访问"); 
    Serial.println();
    delay(500);
    buzzer_Di();
    Serial.println("您好，请进");
    myservo.write(angle2);  //控制舵机转动相应的角度。
    delay(4000);
    myservo.write(angle1);  //控制舵机转动相应的角度。
  }
 
 else   {
    Serial.println("拒绝访问");
    myservo.write(angle1);  //控制舵机转动相应的角度。
    delay(100); //延时100毫秒
    buzzer_Di();
    delay(100); //延时100毫秒
    buzzer_Di();
  }
}
//蜂鸣器函数。
void buzzer_Di() 
{
  digitalWrite(BUZZER,LOW);
  delay(300);
  digitalWrite(BUZZER,HIGH);
}
