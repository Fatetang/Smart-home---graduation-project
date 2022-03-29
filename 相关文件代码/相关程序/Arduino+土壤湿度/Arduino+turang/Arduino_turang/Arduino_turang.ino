 
/*
 * 传感器-8
 * 有水   LOW     0
 * 无水   HIGH    1
 * 
 * 继电器-13
 * 低电平触发  LOW   0
 *
 */
 
#include <Servo.h>
 
int sensorpin = 6;//传感器引脚 有水为0；无水为1
float pumpin = 13;//水泵引脚 低电平触发 digitalWrite（8，LOW）；
int var;
int var3;
 
int sensorpin2 = A5;
 
void setup() {
  Serial.begin(9600);
  
  pinMode(sensorpin,INPUT);
  pinMode(sensorpin2,INPUT);
  pinMode(pumpin,OUTPUT);
  digitalWrite(pumpin,LOW);//水泵关闭
}
 
void loop() {
 
     var = digitalRead(sensorpin);
     
     int var2=digitalRead(pumpin);
     var3=analogRead(sensorpin2);
     
    Serial.print("var3:");
    Serial.println(var3);
    
  //如果传感器检测值为1表示没水;0表示有水
  if(var==1&&var3>600)
  {
    Serial.print(var); 
    Serial.println("缺水状态！");
    digitalWrite(pumpin,HIGH);//水泵打开
    Serial.println("水泵状态-打开");
     
   delay(5000);//浇水的时间是10秒
   
    digitalWrite(pumpin,LOW);//水泵关闭
     Serial.println("水泵-关闭");
    
 
    }
    else
    {
    digitalWrite(pumpin,LOW);//水泵关闭
    Serial.print(var); 
    Serial.println("不缺水状态！");
    Serial.println("水泵-关闭");
   
    }
 
 
   delay(6000);//十秒钟扫描一次；
     //delay(36000000);//10小时扫描一次
 
}
