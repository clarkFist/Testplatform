#_*_coding:utf-8_*_ 

import re #正则表达式库
import threading
from datetime import datetime, timedelta
import time
import sys, getopt
from hashlib import md5
import os
import sqlite3
from sub_module_base import SubMoudleBase
class DataBase(SubMoudleBase):
    '''description:用于储存从串口读取的数据
    
    Attributes: log,path，V_xxxxyyy,logview,oneview
    
    Arguments:
      log:tabble 储存所有读取到的数据\n
      logpath:tabble 储存读取到的最新数据\n
      V_xxxxyyy:view 来自路径xxxx行号为yyy的某段时间的视图\n
      logview:view 查看某段时间的所有数据\n
      oneview:view 查看自最新的一条记录前n 分钟的，来自不同路径行号的数据
    '''
    
    #创建表log时执行的sql
    logform = '''create table if not exists log
            (DATETIME char[30],
             NUM NUMBER,
             CPU char[5],
             PATH char[60],
             LINE NUMBER,
             MSG TEXT,
             TIME NUMBER,
             PRIMARY Key(TIME,NUM));'''
    #创建表logpath 的sql
    pathform = '''create table if not exists logpath
            (DATETIME char[30],
             NUM NUMBER,
             CPU char[5],
             PATH char[60],
             LINE NUMBER,  
             MSG TEXT,
             TIME INTEGER,
             PRIMARY Key(PATH,LINE));'''
    #对log插入数据执行的sql
    loginsert = '''insert into log values (?,?,?,?,?,?,?)'''
    #对pathlog插入数据执行的sql
    pathinsert = '''insert or replace into logpath values (?,?,?,?,?,?,?)'''

    def __init__(self, path, **kwargs):
        self.path = path
        self.tmp = []
        self._isopen = False
        
    def createTable1(self, cmd):
        self.open()
        self.cur.execute(cmd)
        self.commit()
        self.close()

    def log_insert(self, args):
        cmd = DataBase.loginsert
        self.tmp.append(str(args))
        if len(self.tmp) > 30:
            self.tmp = self.tmp[-10:]

        try:
            self.cur.execute(cmd, args)
        except sqlite3.IntegrityError as e:
            self.print(repr(e), self.path, 'key repeat', mode = 1)
            #self.print('\r\n' + '\r\n'.join(self.tmp), mode = 3)

    def path_insert(self, args):
        cmd = DataBase.pathinsert
        try:
            self.cur.execute(cmd, args)
        except Exception as e:
            raise e

    def commit(self):
        self._conn.commit()

    def close(self):
        self._isopen = False
        self.cur.close()
        self._conn.close()
        # self.filelog.close()

    def open(self):
        if self._isopen is True:
            return False
        self._isopen = True
        dirname = os.path.dirname(self.path)
        if not os.path.exists(dirname):
            os.makedirs(dirname)
        self._conn = sqlite3.connect(self.path)
        self.cur = self._conn.cursor()
        return True
        # if self.path != ':memory:':
        #     self.filelog = open(self.path+'.log','a+')
        # else:
        #     self.filelog = open('log/memory.log','a+')
        
# # 将字符串按\r\n分割
# def getline(data):
#     '''将字符串按'\n'分割,并将最后一行与下一次的参数合并'''
#     if not hasattr(getline, 'buf'):
#         getline.buf = str('')
#     data = str(data)#将bytes 编码为utf-8字符串
#     data = re.sub('\x1b[^mABCDHJKsuIh]*[mABCDHJKsuIh]','',getline.buf + data)#去除终端控制符
#     data = re.sub('\r', '', data)
#     data = re.sub('\n\n', '\n', data)
#     lines = data.split('\n')
#     getline.buf = lines.pop()#将可能未处理完成的最后一行与下一次读取的数据合并处理
#     return lines

#返回当前时间 '%Y-%m-%d %H:%M:%S'
def gettime():
    '''fmt:%Y-%m-%d %H:%M:%S'''
    return datetime.now().strftime('%Y-%m-%d %H:%M:%S')
def get_num_gene():
    while True:
        for i in range(1000):
            yield int(-i)
get_num = get_num_gene()
#处理单行数据
def dbcommit(db: DataBase, lines, single = True):
    
    # if single is False:
    #     for line in lines:
    #         if len(lines) > 0:
    #             dbcommit(line, True)
    
    if not db.open():#打开数据库连接
        print('re search fail', lines)
        return 
    line = str(lines)
    args = []
    args.append(gettime())
    res = re.search(r'(\d+)\s+([CPUABchanel12]+)\s+(\S+):(\d+)>(.*)', line)
    if res is None:
        res = re.search(r'(\d+)\s+([CPUABchanel12]+)\s+\w+\s+(\S+):(\d+)>(.*)', line)

    if res is not None:#若匹配成功则按格式保存
        if db.path != ':memory:':
            for i in range(1, 6):
                args.append(res.group(i))
            args.append(int(time.time()))
            args[1] = int(args[1])
            args[4] = int(args[4])
            #print(args)
            db.log_insert(tuple(args))
            db.path_insert(tuple(args))
    else:
        if db.path != ':memory:':
            args.append( next(get_num) )
            args.append('CPU')
            args.append('print')
            args.append( 0 )
            args.append( line )
            args.append(int(time.time()))
            db.log_insert(tuple(args))
            db.path_insert(tuple(args))
    db.commit()
    db.close()
#返回字符串的md5值
def strid(s):
    id = 0
    s = bytes(s, encoding = "utf8")
    id = md5(s).hexdigest()
    return id  

if __name__ == '__main__':

    db = DataBase(r'C:\Users\LiuXintian\Desktop\testplatform\Log_Log\database\abcd.db')
    db.createTable1(db.logform)  #创建表
    db.createTable1(db.pathform)
    dbcommit(db, 'asd')