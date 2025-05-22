# !/usr/bin/python
# _*_coding:utf-8_*_
import unittest
import os, sys
import re
sys.path.append(os.path.abspath(os.getcwd()))
import udp_loglib
from udp_loglib import Public,VCU,UdpRecvFrame,UdpSendFrame,Default,Log
class PublicTest(unittest.TestCase):
    def test_modify1(self):
        string = '123456'
        string = Public.modify(string,'3-4','43')
        self.assertEqual(string,'1234356')
    def test_modify2(self):
        string = '123456'
        string = Public.modify(string,range(3,6),'43')
        self.assertEqual(string,'12343')
    def test_tuple_len(self):
        tu = (123,'123',(),('123',1),['123'],'')
        le = Public.tuple_len(tu)
        self.assertEqual(le,13)
    def test_int2hex(self):
        val = 0xFFFFFFFF
        val = Public.int2hex(val,8)
        self.assertEqual(val,'FFFFFFFF')
        val = 0xFFFFFFF
        val = Public.int2hex(val,8)
        self.assertEqual(val,'0FFFFFFF')
        val = 1
        val = Public.int2hex(val,2)
        self.assertEqual(val,'01')
        # val = 0xFFFFFFFF
        # try:
        #     val = Public.int2hex(val,7)
        # except SystemExit as e:
        #     val = ''
        # self.assertEqual(val,'')
class VCUTest(unittest.TestCase):
    def test_moudle_3toNode(self):
        nodes = VCU.moudle_3toNode(2)
        self.assertEqual(nodes,('02','03'))
        nodes = VCU.moudle_3toNode(3)
        self.assertEqual(nodes,('0c','0d'))
    def test_inputsplit(self):
        res = VCU.inputsplit('1,2,3-8.9,10,12,15')
        res.sort()
        self.assertEqual(res,[2,3,4,5,6,7,8,9,10,12])
        res = VCU.inputsplit('17,18,19,34,31')
        res.sort()
        self.assertEqual(res,[18,19,34])
        res = VCU.inputsplit('3-8,25',2)
        res.sort()
        self.assertEqual(res,[19,20,21,22,23,24,25])
    def test_node2lru(self):
        lru = VCU.node2lru(38)
        self.assertEqual(lru,('1_16S','CPUA'))
        lru = VCU.node2lru(3)
        self.assertEqual(lru,('1_2P','CPUB'))
        lru = VCU.node2lru(5)
        self.assertEqual(lru,('1_16S','CPUB'))
        lru = VCU.node2lru(10)
        self.assertEqual(lru,('1_2P','CPUA'))
        lru = VCU.node2lru(12)
        self.assertEqual(lru,('1_3','CPUA'))
        lru = VCU.node2lru('0C')
        self.assertEqual(lru,('1_3','CPUA'))

class UdpSendFrameTest(unittest.TestCase):
    def setUp(self):
        class tst(Default):
            _mtype = '12'
        self.clas = tst
    def test_toUdpData(self):
        frame = UdpSendFrame()
        frame.command = '12345678'
        frame.mtype = self.clas
        frame.srcNodeID = '00'
        frame.dstNodeID = 'FF'
        self.assertEqual(frame.data,'FF00120812345678')
class UdpRecvFrameTest(unittest.TestCase):
    def setUp(self):
        self.res = UdpRecvFrame('FF120e2d00007473745f74736b2e633a3239363e007f0500040000001200000012000000000000000000000000')
        self.res.data_type._default_run(self.res)
    def test_UdpRecvFrame1(self):
        res = self.res
        self.assertEqual(res.destNodeID,'FF')
        self.assertEqual(res.srcNodeID,'12')
        self.assertEqual(res.dataType,'0E')
        self.assertEqual(res.dataLen,'2D')
    def test_UdpRecvFrame2(self):
        # self.assertEqual(Log.src,'12')
        self.assertEqual(Log.src_lru,'1_6')
        self.assertEqual(Log.src_cpu,'CPUA')
        self.assertTrue(isinstance(Log.number,int))
        res = re.sub(r'^\w+\.[ch]:\d+.*$','',Log.info)
        self.assertEqual(res,'')
        self.assertEqual(len(Log.recvVal),6)
if __name__ == "__main__":
    unittest.main()