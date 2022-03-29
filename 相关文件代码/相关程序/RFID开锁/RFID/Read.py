#!/usr/bin/env python
# -*- coding: utf8 -*-
#
#    Copyright 2014,2018 Mario Gomez <mario.gomez@teubi.co>
#
#    This file is part of MFRC522-Python
#    MFRC522-Python is a simple Python implementation for
#    the MFRC522 NFC Card Reader for the Raspberry Pi.
#
#    MFRC522-Python is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    MFRC522-Python is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with MFRC522-Python.  If not, see <http://www.gnu.org/licenses/>.
#

import RPi.GPIO as GPIO
import MFRC522
import signal
import time

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

# 捕获SIGINT以便在脚本中止时进行清理
def end_read(signal,frame):
    global continue_reading
    print "Ctrl+C captured, ending read."
    continue_reading = False
    GPIO.cleanup()

# 钩住SIGINT
signal.signal(signal.SIGINT, end_read)

# 创建一个类MFRC522的对象
MIFAREReader = MFRC522.MFRC522()

# Welcome message
print "Welcome to the MFRC522 data read example"
print "Press Ctrl-C to stop."

# 该循环不断检查芯片。 如果附近有，它将获取UID并进行身份验证
while continue_reading:
    
    # 扫描卡片   
    (status,TagType) = MIFAREReader.MFRC522_Request(MIFAREReader.PICC_REQIDL)

    # 如果找到卡
    if status == MIFAREReader.MI_OK:
        print "检测到卡"
    #获取卡的UID
    (status,uid) = MIFAREReader.MFRC522_Anticoll()

    #如果我们有UID，请继续
    if status == MIFAREReader.MI_OK:

        # 打印UID
        print "读卡UID: %s,%s,%s,%s" % (uid[0], uid[1], uid[2], uid[3])
    
        # 这是身份验证的默认密钥
        key = [0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]
        
        # 选择扫描的标签
        MIFAREReader.MFRC522_SelectTag(uid)

        # 认证
        status = MIFAREReader.MFRC522_Auth(MIFAREReader.PICC_AUTHENT1A, 8, key, uid)
        
        #检查是否通过身份验证
        if status == MIFAREReader.MI_OK:
            MIFAREReader.MFRC522_Read(8)
            MIFAREReader.MFRC522_StopCrypto1()
            init()
            beepBatch(0.3,0.3,1)
        else:
            print "授权错误"
            init()
            beepBatch(0.3,0.3,3)

