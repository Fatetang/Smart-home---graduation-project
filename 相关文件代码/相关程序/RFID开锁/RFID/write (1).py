#!/usr/bin/env python

import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
import time

reader = SimpleMFRC522()
continue_reading = True
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

def write():
        text = input('New data:')
        print("现在放置您的标签来写")
        reader.write(text)
        print("成功")

def gpio():
        GPIO.cleanup()

# 主函数
if __name__ == '__main__':
    while continue_reading:
          init()
          write()
          beepBatch(0.3,0.3,1)
          gpio()
