#!/usr/bin/env python

import RPi.GPIO as GPIO
import numpy as np
from mfrc522 import SimpleMFRC522
from time import sleep
import time

#GPIO调用及接口
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)

servo = 16

reader = SimpleMFRC522()
UID = [649454335083,342918131572]
arr = np.array(UID)

GPIO.setup(servo, GPIO.OUT)

#控制舵机角度
def setServoAngle(servo, angle):
    pwm = GPIO.PWM(servo, 50)
    pwm.start(8)
    dutyCycle = angle / 18. + 3.
    pwm.ChangeDutyCycle(dutyCycle)
    sleep(0.3)
    pwm.stop()
    
#有源蜂鸣器模块
#哔N次，时长、间隔时长、重复次数作为参数传递
def init():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BOARD) 
    GPIO.setup(11,GPIO.OUT,initial=GPIO.HIGH)
    pass

def beep(seconds):
    GPIO.output(11,GPIO.LOW) 
    time.sleep(seconds)
    GPIO.output(11,GPIO.HIGH) 
    
def beepBatch(seconds,timespan,counts):
    for i in range(counts):
        beep(seconds)
        time.sleep(timespan) 

def read():
        id, text = reader.read()
        print(id)
        print(text)


# 主函数
if __name__ == '__main__':
    while True:
    # 如果找到卡
       init()
       print("请把IC卡放在感应区")
       id, text = reader.read()
       if (arr == id).any() ==  True:
          print("IC卡已授权,欢迎主人!!")
          print("开门")
          beepBatch(0.3,0.3,1)
          setServoAngle(servo,180)
          time.sleep(2)
          setServoAngle(servo,0)          
       else:
          print("IC卡未授权，请先授权后再试！")
          beepBatch(0.3,0.3,3)