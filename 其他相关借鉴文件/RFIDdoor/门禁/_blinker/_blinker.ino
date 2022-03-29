/*
 * @Descripttion: 
 * @version: 
 * @Author: QYH
 * @Date: 2019-09-12 16:36:28
 * @LastEditors: QYH
 * @LastEditTime: 2019-09-29 15:03:45
 */
#define BLINKER_WIFI
#define BLINKER_PRINT Serial
#define BLINKER_DUEROS_MULTI_OUTLET
#include <Blinker.h>

char auth[] = "..";
char ssid[] = "..";
char pswd[] = "..";
bool oState[5] = {false};

//灯的状态 0-待命 1-开 2-关
uint8_t State_Light_S = 0;
uint8_t State_Light_N = 0;

// 新建组件对象
BlinkerButton Button_door_key("btn-door1");
BlinkerButton Button_gun_key("btn-gun"); //驱逐模式
BlinkerButton Button_light_s_switch("btn-light-s");
BlinkerButton Button_light_n_switch("btn-light-n");

extern bool Door_Open_Flag;

void callback_Button_door_key(const String &state)
{
    BLINKER_LOG("get button state: ", state);
    if (state == "tap")
    {
        Door_Open_Flag = 1;
    }
}

void callback_Button_gun_key(const String &state)
{
    BLINKER_LOG("get button state: ", state);
    if (state == "tap")
    {
    }
}

void callback_Button_light_s_switch(const String &state)
{
    BLINKER_LOG("get button state s: ", state);
    if (state == "on")
    {
        State_Light_S = 1;
        // 反馈开关状态
        //Button_light_s_switch.print("on");
    }
    else if (state == "off")
    {
        State_Light_S = 2;
        // 反馈开关状态
        // Button_light_s_switch.print("off");
    }
}

void callback_Button_light_n_switch(const String &state)
{
    BLINKER_LOG("get button state n: ", state);
    if (state == "on")
    {
        State_Light_N = 1;

        // 反馈开关状态
        //Button_light_n_switch.print("on");
    }
    else if (state == "off")
    {
        State_Light_N = 2;
        // 反馈开关状态
        //Button_light_n_switch.print("off");
    }
}

//小度
void duerPowerState(const String &state, uint8_t num)
{
    BLINKER_LOG("need set outlet: ", num, ", power state: ", state);

    if (state == BLINKER_CMD_ON)
    {

        // BlinkerDuerOS.powerState("on", num);
        // BlinkerDuerOS.print();

        oState[num] = true;

        if (0 == num)
        {
            State_Light_S = 1;
        }
        else if (1 == num)
        {
            State_Light_N = 1;
        }
    }
    else if (state == BLINKER_CMD_OFF)
    {

        // BlinkerDuerOS.powerState("off", num);
        // BlinkerDuerOS.print();

        oState[num] = false;

        if (0 == num)
        {
            State_Light_S = 2;
        }
        else if (1 == num)
        {
            State_Light_N = 2;
        }
    }
}

void Blinker_Report_Light_State(uint8_t light_s, uint8_t light_n)
{

    if (1 == light_s)
    {
        BlinkerDuerOS.powerState("on", 0);
        BlinkerDuerOS.print();
        Button_light_s_switch.print("on");
    }
    else if (2 == light_s)
    {
        BlinkerDuerOS.powerState("off", 0);
        BlinkerDuerOS.print();
        Button_light_s_switch.print("off");
    }

    if (1 == light_n)
    {
        BlinkerDuerOS.powerState("on", 1);
        BlinkerDuerOS.print();
        Button_light_n_switch.print("on");
    }
    else if (2 == light_n)
    {
        BlinkerDuerOS.powerState("off", 1);
        BlinkerDuerOS.print();
        Button_light_n_switch.print("off");
    }
}
