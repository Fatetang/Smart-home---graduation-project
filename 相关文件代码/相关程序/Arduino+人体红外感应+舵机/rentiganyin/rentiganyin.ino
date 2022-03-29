#include <Servo.h> //调用库文件
Servo myserv0;  // 创建一个伺服电机对象 
Servo myserv1;  // 创建一个伺服电机对象

int angle1 = 90;  //原始角度值 
int angle2 = 0;  //旋转角度值
int Sensor_pin1 = A5;
int angle3 = 0;  //原始角度值 
int angle4 = 90;  //旋转角度值
int Sensor_pin2 = A4;

void setup()
{
    pinMode(Sensor_pin1,INPUT); //设置人体红外接口为输入状态
    pinMode(Sensor_pin2,INPUT); //设置人体红外接口为输入状态
    myserv0.attach(8);    //定义舵机的引脚为8
    myserv1.attach(9);    //定义舵机的引脚为9
    Serial.begin(9600);
}
void loop()
{
    int val0=digitalRead(Sensor_pin1); //定义参数存储人体红外传感器读到的状态
    int val1=digitalRead(Sensor_pin2); //定义参数存储人体红外传感器读到的状态
    
    Serial.println(val0);
    Serial.println(val1);
    if(val0 == 1) //如果检测到有人（在检测范围内）
    {
      myserv0.write(angle2);  //控制舵机转动相应的角度。
    }
    else
    {
      myserv0.write(angle1);  //控制舵机转动相应的角度。
      delay(100); //延时100毫秒
    }
    if(val1 == 1) //如果检测到有人（在检测范围内）
    {
      myserv1.write(angle4);  //控制舵机转动相应的角度。
    }
    else
    {
      myserv1.write(angle3);  //控制舵机转动相应的角度。
    }
    delay(100); //延时100毫秒
}
