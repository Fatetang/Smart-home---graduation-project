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

continue_reading = True

# Capture SIGINT for cleanup when the script is aborted
def end_read(signal,frame):
    global continue_reading
    print "Ctrl+C captured, ending read."
    continue_reading = False
    GPIO.cleanup()

# Hook the SIGINT
signal.signal(signal.SIGINT, end_read)

# Create an object of the class MFRC522
MIFAREReader = MFRC522.MFRC522()

# 该循环不断检查芯片。 如果附近有，它将获取UID并进行身份验证
while continue_reading:
    
    # 扫描卡片
    (status,TagType) = MIFAREReader.MFRC522_Request(MIFAREReader.PICC_REQIDL)

    # 如果找到卡
    if status == MIFAREReader.MI_OK:
        print "检测到卡"
    
    #获取卡的UID
    (status,uid) = MIFAREReader.MFRC522_Anticoll()

    # 如果我们有UID，请继续
    if status == MIFAREReader.MI_OK:

        #打印UID
        print "读卡UID: %s,%s,%s,%s" % (uid[0], uid[1], uid[2], uid[3])
    
        # 这是身份验证的默认密钥
        key = [0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]
        
        #选择扫描的标签
        MIFAREReader.MFRC522_SelectTag(uid)

        # 认证
        status = MIFAREReader.MFRC522_Auth(MIFAREReader.PICC_AUTHENT1A, 8, key, uid)
        print "\n"

        # 检查是否通过身份验证
        if status == MIFAREReader.MI_OK:

            # 数据写入变量
            data = [0x01,0x02,0x03,0xDF,0xEF,0xFF]

            # 用0xFF填充数据
            for x in range(0,16):
                data.append(0xFF)

            print "8区看起来像这样:"
            #读块8
            MIFAREReader.MFRC522_Read(8)
            print "\n"

            print "现在，扇区8将充满0xFF:"
            # 写数据
            MIFAREReader.MFRC522_Write(8, data)
            print "\n"

            print "现在看起来像这样:"
            # 检查是否已编写
            MIFAREReader.MFRC522_Read(8)
            print "\n"

            data = [0x01,0x02,0x03,0xDF,0xEF,0xFF]
            #用0x00填充数据
            for x in range(0,16):
                data.append(0x00)

            print "现在我们用0x00填充:"
            MIFAREReader.MFRC522_Write(8, data)
            print "\n"

            print "现在是空的:"
            # 检查是否已编写
            MIFAREReader.MFRC522_Read(8)
            print "\n"

            # 停止
            MIFAREReader.MFRC522_StopCrypto1()

            # 确保停止读取卡
            continue_reading = False
        else:
            print "授权错误"
