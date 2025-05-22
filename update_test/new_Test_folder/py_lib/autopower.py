#!/usr/bin/python
#coding=utf-8
# ----------------------------------------------------------------------------
# FileName:     autopower.py
# Description:  自动电源控制模块   
# Author:       Xiongkunpeng
# Version:      0.0.1
# Created:      2014-03-17
# Company:      CASCO
# LastChange:   update 2014-03-17
# History:                  
# ---------------------------------------------------------------------------
from __future__ import print_function
import socket
import struct
import select
import time
import sys
if sys.version_info < (3, 0):
    version = 2
else:
    version = 3

class AutoPower(object):
    "auto power"
    __dstIP = "172.16.0.2"
    __dstPort = 1200
    __srcIP = "192.100.89.202"
    __srcPort = 1200
    __socket = None
    __subtype = 1
    _sendOnData = []
    _sendOffData = []

    def __init__(self):
        "init"
        pass
    @classmethod
    def createSocket(cls):
        '''create socket'''
       # print("create socket")
        cls.__socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        cls.__socket.settimeout(5)
        
    # self.__socket.setblocking(0)#设置为非阻塞模式
    @classmethod
    def sendHeartBeat(cls):
        return True

    @classmethod
    def getSendData(cls, channel):
        if channel == "1":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x00010000, 0x9216]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x00010000, 0x9316]
        if channel == "2":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x00020000, 0x9316]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x00020000, 0x9416]
        if channel == "3":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x00040000, 0x9516]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x00040000, 0x9616]
        if channel == "4":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x00080000, 0x9916]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x00080000, 0x9a16]
        if channel =="1,2":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x00030000, 0x9416]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x00030000, 0x9516]
        if channel == "1,3":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x00050000, 0x9616]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x00050000, 0x9716]
        if channel == "1,4":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x00090000, 0x9a16]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x00090000, 0x9b16]
        if channel == "2,3":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x00060000, 0x9716]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x00060000, 0x9816]
        if channel == "2,4":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x000a0000, 0x9b16]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x000a0000, 0x9c16]
        if channel == "3,4":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x000c0000, 0x9d16]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x000c0000, 0x9e16]
        if channel == "1,2,4":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x000b0000, 0x9c16]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x000b0000, 0x9d16]
        if channel == "1,3,4":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x000d0000, 0x9e16]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x000d0000, 0x9e16]
        if channel == "2,3,4":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x000f0000, 0xa016]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x000f0000, 0xa116]
        if channel == "1,2,3,4":
            cls._sendOnData = [0xffffffff, 0x14000001, 0x00000000, 0x81ff0000, 0x000f0000, 0xa016]
            cls._sendOffData = [0xffffffff, 0x14000001, 0x00000000, 0x81000000, 0x000f0000, 0xa116]

    @classmethod
    def sendPowerOnCommand(cls, channel):
        "send power on command"
        # 源socket可写
        AutoPower.createSocket()
        AutoPower.getSendData(channel)
        _sockList = [cls.__socket]
        _rs, _ws, _es = select.select([], _sockList, [])
        _Mes = struct.pack("!5IH", *(cls._sendOnData))

        time_try = 10
        while time_try > 0:
            try:
                _ws[0].sendto(_Mes, (cls.__dstIP, cls.__dstPort))
                data, addr = cls.__socket.recvfrom(100)
                if version == 3:
                    if data[4] == 0x94 and data[13] == 0xff:
                        print("POWER ON")
                        break
                else:
                    if ord(data[4]) == 0x94 and ord(data[13]) == 0xff:
                        print("POWER ON")
                        break
                time.sleep(1)
                time_try -= 1
            except socket.error as e:
                #print('socket:'+ _ws[0].getsockname()+'SendHeartBeat'+e)
                #return None
                raise e
        AutoPower.closeSocket()
        if time_try == 0:
            raise Exception()
        return True

    @classmethod
    def sendPowerOffCommand(cls, channel):
        "send power on command"
        AutoPower.createSocket()
        _sockList = [cls.__socket]
        _rs, _ws, _es = select.select([], _sockList, [])
        AutoPower.getSendData(channel)
        _Mes = struct.pack("!5IH", *(cls._sendOffData))
        time_try = 15
        while time_try > 0:
            try:
                _ws[0].sendto(_Mes, (cls.__dstIP, cls.__dstPort))
                data, addr = cls.__socket.recvfrom(100)
                #data = ord(data)
                if version == 3:
                    if data[4] == 0x94 and data[13] == 0x00:
                        print("POWER OFF")
                        break
                else:
                    if ord(data[4]) == 0x94 and ord(data[13]) == 0x00:
                        print("POWER OFF")
                        break
                time.sleep(1)
                time_try -= 1
            except socket.error as e:
                print('socket:'+ str(_ws[0].getsockname()) + 'SendHeartBeat'+str(e))
                return None
        AutoPower.closeSocket()
        if time_try == 0:
            raise Exception()
        return True

    @classmethod
    def closeSocket(self):
        "close socket"
        self.__socket.close()


if __name__ == '__main__':
    if True:
        AutoPower.sendPowerOffCommand("1")
        time.sleep(2)
        AutoPower.sendPowerOnCommand("1,2")
