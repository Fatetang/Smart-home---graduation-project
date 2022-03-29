/*
 * @Descripttion: 
 * @version: 
 * @Author: QYH
 * @Date: 2019-09-16 17:15:57
 * @LastEditors: QYH
 * @LastEditTime: 2019-09-22 16:12:09
 */
#include <Servo.h>
#include <Ticker.h>

#define Door_Open_Angle 156
#define Door_Close_Angle 78
#define Door_Servo_Pin 5

#define Light_S_Open_Angle 118
#define Light_S_Close_Angle 69
#define Light_S_Ready_Angle 90
#define Light_S_Servo_Pin 4

#define Light_N_Open_Angle 78
#define Light_N_Close_Angle 129
#define Light_N_Ready_Angle 103
#define Light_N_Servo_Pin 0

Servo Servo_Door;    // 创建舵机对象来控制舵机
Servo Servo_Light_S; // 创建舵机对象来控制舵机
Servo Servo_Light_N; // 创建舵机对象来控制舵机

Ticker Light_Ready;
Ticker Door_StopOut;

// Servo_Door.attach(15); // 把连接在引脚9上的舵机赋予舵机对象
// Servo_Door.write(90);  // 告诉舵机到达位置变量‘pos’

void Motor_Init()
{
    Servo_Light_S.attach(Light_S_Servo_Pin);
    Servo_Light_S.write(Light_S_Close_Angle);

    Servo_Light_N.attach(Light_N_Servo_Pin);
    Servo_Light_N.write(Light_N_Close_Angle);

    Light_Ready.once_ms(700, callback_Light_Ready);

    //Servo_Door.attach(Door_Servo_Pin);
    // Servo_Door.write(Door_Close_Angle);
}

void Light_Event(uint8_t light_s, uint8_t light_n)
{
    if (1 == light_s)
    {
        Servo_Light_S.write(Light_S_Open_Angle);
        Serial.println(F("s on"));
    }
    else if (2 == light_s)
    {
        Servo_Light_S.write(Light_S_Close_Angle);
        Serial.println(F("s off"));
    }

    if (1 == light_n)
    {
        Servo_Light_N.write(Light_N_Open_Angle);
        Serial.println(F("n on"));
    }
    else if (2 == light_n)
    {
        Servo_Light_N.write(Light_N_Close_Angle);
        Serial.println(F("n off"));
    }

    if (0 != light_s || 0 != light_n)
    {
        Light_Ready.once_ms(700, callback_Light_Ready);
    }
}

void callback_Light_Ready()
{
    Servo_Light_S.write(Light_S_Ready_Angle);
    Servo_Light_N.write(Light_N_Ready_Angle);
}

//state 0 关闭输出  1 关门舵机角度
//关闭输出后钥匙可以转的动   //有的舵机可以
void callback_Door_StopOut(int state)
{
    if (0 == state)
    {
        Servo_Door.detach();
    }
    else if (1 == state)
    {
        Servo_Door.write(Door_Close_Angle);
        Door_StopOut.once_ms(1000, callback_Door_StopOut, 0);
    }
}