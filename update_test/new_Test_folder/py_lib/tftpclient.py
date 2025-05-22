# -*- coding:utf-8 -*-

import socket
import struct
import sys
import binascii

class TftpException(Exception):
    pass

class TftpClient:
    """
        下载基本流程：
        --------------------------------------
        客户端(Client)         服务端(Server)
        --------------------------------------
        读写请求        --->
                        <---     数据包[0]
        ACK[0]          --->
                        <---      数据包[1]
        ACK[1]          --->
        ....
        --------------------------------------

        操作码     功能
        --------------------------------------
        1           读请求，即下载
        2           写请求，即上传
        3           表示数据包，即Data
        4           确认码，即ACK
        5           错误
        --------------------------------------
    """
    def __init__(self, remoteIp = '20.2.1.10'):
        
        # 创建socket实例
        self.socketClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        #self.socketClient.bind(('', local_port))
        self.socketClient.settimeout(3)
        self.remoteIp = remoteIp

    def read_request(self, dstfilename, port = 69, srcfilename= None):
        """ TFTP 下载"""

        # 构建下载请求数据
        # 第一个参数 !H7sb5sb = "!H"+str(len(filename))+"sb5sb"
        cmdBuf = struct.pack(("!H%ssb5sb" % len(dstfilename)).encode("utf-8"), 1, dstfilename.encode("utf-8"), 0, b"octet", 0)

        # 发送下载文件请求数据到指定服务器
        self.socketClient.sendto(cmdBuf, (self.remoteIp, port))

        # self.show()

        locPackNum = 0  # 请求包号
        if srcfilename is not None:
            saveFile = srcfilename   # 保存文件句柄
        else:
            data = b''

        while True:
            recvData, recvAddr = self.socketClient.recvfrom(1024)
            recvDataLen = len(recvData)

            # 解包
            cmdTuple = struct.unpack(b"!HH", recvData[:4])
            cmd = cmdTuple[0]   # 指令
            curPackNum = cmdTuple[1]    # 当前包号

            if cmd == 3:    # 是否为数据包
                if curPackNum == 1:
                    # 以追加的方式打开文件
                    if srcfilename is not None:
                        saveFile = open(srcfilename, "ab")

                # 包编号是否和上次相等
                if locPackNum + 1 == curPackNum:
                    if srcfilename is not None:
                        saveFile.write(recvData[4:])    # 写入数据
                    else:
                        data += recvData[4:]

                    locPackNum += 1

                    # 发送ACK应答
                    ackBuf = struct.pack(b"!HH", 4, locPackNum)
                    self.socketClient.sendto(ackBuf, recvAddr)

                # 确认为最后一个包
                if recvDataLen < 516:
                    if srcfilename is not None:
                        saveFile.close()
                    break

            elif cmd == 5:  # 是否为错误应答
                raise TftpException("error num:%d, %s" % (curPackNum, recvData[4:].decode()) )

        if srcfilename is None:
            return data           

    def write_request(self, dstfilename, port = 69, srcfilename = None, data = None):
        """TFTP 上传"""

        # 1、发送读请求
        cmdBuf = struct.pack(("!H%ssb5sb" % len(dstfilename)).encode("utf-8"), 2, dstfilename.encode("utf-8"), 0, b"octet", 0)

        self.socketClient.sendto(cmdBuf, (self.remoteIp, port))

        localPackNum = 1    # 本地包号
        if srcfilename is not None:
            sendFile = srcfilename   # 保存文件句柄
        elif data is None:
            raise TftpException(srcfilename, data)

        while True:
            # 2、接收回复
            recvData, recvAddr = self.socketClient.recvfrom(1024)
            # 3、解包
            cmdTuple = struct.unpack(b"!HH", recvData[:4])
            cmd = cmdTuple[0]  # 指令
            curPackNum = cmdTuple[1]  # 当前包号

            # print(recvData)

            if cmd == 4:
                # 打开并读取文件
                if curPackNum == 0:
                    if srcfilename is not None:
                        sendFile = open(srcfilename, "rb")
                    
                # ACK应答的包号是否与本地的一样
                if localPackNum - 1 == curPackNum:
                    # 4、读取 512 byte数据
                    if srcfilename is not None:
                        sendData = sendFile.read(512)
                        datalen = 512
                    else:
                        sendData = binascii.unhexlify(data) 
                        datalen = len(data) // 2
                        data = ''
                    # 判断文件是否读取完成
                    if len(sendData) <= 0:
                        if srcfilename is not None:
                            sendFile.close()
                        break
                    
                    # 5、打包发送数据
                    sendDataBuf = struct.pack(("!HH%ss" % datalen).encode('utf8'), 3, localPackNum, sendData)
                    self.socketClient.sendto(sendDataBuf, recvAddr)

                    # 打印过程
                    localPackNum += 1

            elif cmd == 5:
                # sendFile.close()
                raise TftpException("error num:%d %s" % (curPackNum, recvData[4:]))


if __name__ == "__main__":
    demo = TftpClient()
    demo.write_request('cop_v001_open', data='16')
    data = demo.read_request('viv_v001_oHDGB45G6', port = 1464)
    data = binascii.b2a_hex(data).decode()
    print(data)
    data = demo.read_request('viv_v001_i%s' % data, port = 1464)
    data = binascii.b2a_hex(data).decode()
    print(data)