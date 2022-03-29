
/*
 * @Descripttion: 
 * @version: 
 * @Author: QYH
 * @Date: 2019-09-02 16:59:03
 * @LastEditors: QYH
 * @LastEditTime: 2019-09-29 14:58:22
 */
/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Ticker.h>
#include <EEPROM.h>

Ticker BUZZ_Task;
#define BUzz_PIN 16

Ticker Close_Root_Task;

bool read_a_card_flag = 0;
bool Door_Open_Flag = 0;
bool Root_Model = 0;

#define RST_PIN 4 //GPIO4  // Configurable, see typical pin layout above
#define SS_PIN 5  // Configurable, see typical pin layout above

MFRC522 rfid; //(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];
//最大用户数，不包括管理员
#define Max_User_Sum 10
//用于存放用户的ID,前4个字节不变为管理员ID
byte UserUid[Max_User_Sum * 4 + 4] = {0x12, 0x1c, 0x19, 0x1c};
//当前用户的总数
byte User_Sum = 1;

void setup()
{

    //Serial.begin(115200);
    //SPIFFS.begin();

    SPI.begin();     // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522
    delay(500);

    //初始化蜂鸣器
    pinMode(BUzz_PIN, OUTPUT);
    digitalWrite(BUzz_PIN, HIGH);
    //Serial.println(F("start"));

    //读flash的数据，断电不丢
    EEPROM.begin(3000); //Blinker 已经使用了0-2447
    for (uint8_t i = 4; i < Max_User_Sum * 4 + 4; i++)
    {
        UserUid[i] = EEPROM.read(i - 4 + 2500);
    }
    User_Sum = EEPROM.read(Max_User_Sum * 4 + 4 + 2500);
    EEPROM.end();
    if (1 > User_Sum)
    {
        User_Sum = 1;
    }
    else if (Max_User_Sum < User_Sum)
    {
        User_Sum = 1;
    }
    Serial.printf("User_Sum  %d", User_Sum);

    BLINKER_DEBUG.stream(Serial);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    Button_door_key.attach(callback_Button_door_key);
    Button_gun_key.attach(callback_Button_gun_key);
    Button_light_s_switch.attach(callback_Button_light_s_switch);
    Button_light_n_switch.attach(callback_Button_light_n_switch);
    BlinkerDuerOS.attachPowerState(duerPowerState);

    Motor_Init();
}
int p;
void loop()
{
    read_a_card_flag = 0;
    Door_Open_Flag = 0;
    State_Light_S = 0;
    State_Light_N = 0;

    Blinker.run();

    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if (rfid.PICC_IsNewCardPresent())
    {
        // Serial.println(F("newcard"));
        if (rfid.PICC_ReadCardSerial())
        {
            read_a_card_flag = 1;

            //Serial.println(F("The NUID tag is:"));
            //Serial.print(F("In hex: "));
            // printHex(rfid.uid.uidByte, rfid.uid.size);

            // Halt PICC
            rfid.PICC_HaltA();

            // Stop encryption on PCD
            rfid.PCD_StopCrypto1();
        }
    }

    if (1 == read_a_card_flag)
    {
        //验证用户
        uint8_t tempValidation = 0;
        tempValidation = Validation_ID(User_Sum, UserUid);

        //验证开门
        if (0 == Root_Model)
        {

            if (0 != tempValidation)
            {
                Door_Open_Flag = 1;
                //蜂鸣器响一声
                digitalWrite(BUzz_PIN, LOW);
                BUZZ_Task.once_ms(300, setPin, HIGH);
            }
            else
            {
                //蜂鸣器响一长声
                digitalWrite(BUzz_PIN, LOW);
                BUZZ_Task.once_ms(1000, setPin, HIGH);
            }
        }

        if (0 == Root_Model && 2 == tempValidation) //开启管理模式
        {
            Root_Model = 1;
            //跳过关闭管理模式
            tempValidation = 3;
            //添加完用户后5分钟自动关闭管理员模式
            Close_Root_Task.once_scheduled(300, Close_Root);

            Serial.println(F("root_open"));
        }
        if (1 == Root_Model && 2 == tempValidation) //关闭管理模式
        {
            Root_Model = 0;
            Close_Root();

            Serial.println(F("root_close"));
        }
        //添加新用户
        if (1 == Root_Model && 0 == tempValidation)
        {
            Add_User(User_Sum, UserUid, rfid.uid.uidByte);
            //添加完用户后5分钟自动关闭管理员模式
            Close_Root_Task.once_scheduled(300, Close_Root);
            User_Sum++;
            if (Max_User_Sum < User_Sum)
            {
                User_Sum = Max_User_Sum;
            }
            Serial.printf("add_new_user   user_sum %d\n", User_Sum);
        }
    }

    //开门
    if (1 == Door_Open_Flag)
    {
        // //蜂鸣器响一声
        // digitalWrite(BUzz_PIN, LOW);
        // BUZZ_Task.once_ms(300, setPin, HIGH);
        Serial.println(F("the door open"));
        Servo_Door.attach(Door_Servo_Pin);
        Servo_Door.write(Door_Open_Angle);
        Door_StopOut.once_ms(3000, callback_Door_StopOut, 1);
    }

    Light_Event(State_Light_S, State_Light_N);
    Blinker_Report_Light_State(State_Light_S, State_Light_N);
    /* code */
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

//验证ID
//返回1 通过
//返回2 管理员卡
uint8_t Validation_ID(byte user_sum, const byte user_uid[])
{
    if (Max_User_Sum <= user_sum)
    {
        user_sum = Max_User_Sum;
    }
    if (1 > user_sum)
    {
        user_sum = 1;
    }

    for (uint8_t i = 0; i <= user_sum; i++)
    {
        if (user_uid[i * 4 + 0] == rfid.uid.uidByte[0])
        {
            if (user_uid[i * 4 + 1] == rfid.uid.uidByte[1] &&
                user_uid[i * 4 + 2] == rfid.uid.uidByte[2] &&
                user_uid[i * 4 + 3] == rfid.uid.uidByte[3])
            {
                if (0 == i)
                {
                    return 2;
                }

                return 1; //验证通过
            }
        }
    }
    return 0;
}

void setPin(int state)
{
    digitalWrite(BUzz_PIN, state);
}

//关闭管理员模式，保存数据到flash
void Close_Root()
{
    EEPROM.begin(8102); //Blinker 已经使用了0-2447
    Root_Model = 0;
    for (uint8_t i = 4; i < Max_User_Sum * 4 + 4; i++)
    {
        EEPROM.write(i - 4 + 2500, UserUid[i]);
    }
    EEPROM.write(Max_User_Sum * 4 + 4 + 2500, User_Sum);
    EEPROM.end();
    Serial.printf(" user_sum %d\n", User_Sum);
}

//添加新用户
void Add_User(byte user_sum, byte user_uid[], const byte new_user[4])
{
    if (Max_User_Sum <= user_sum)
    {
        user_sum = Max_User_Sum;
    }
    if (1 > user_sum)
    {
        user_sum = 1;
    }
    user_uid[user_sum * 4 + 0] = new_user[0];
    user_uid[user_sum * 4 + 1] = new_user[1];
    user_uid[user_sum * 4 + 2] = new_user[2];
    user_uid[user_sum * 4 + 3] = new_user[3];
}

void Buzz_1()
{
    //蜂鸣器响一声
    digitalWrite(BUzz_PIN, LOW);
    BUZZ_Task.once_ms(300, setPin, HIGH);
}
