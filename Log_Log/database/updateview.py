#_*_coding:utf-8_*_ 
from __future__ import print_function
import time
import os, sys, getopt
from datetime import datetime, timedelta
import traceback
import sqlite3
import io
import re #正则表达式库
import threading
class DataBase_write():
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

        def get_num_gene():
            i = 0
            while True:
                yield i
                i += 1
        self.get_num = get_num_gene()
        self.print = print
        
    def createTable1(self, cmd):
        self.open()
        self.cur.execute(cmd)
        self.commit()
        self.close()

    def log_insert(self, args):
        cmd = self.loginsert
        self.tmp.append(str(args))
        if len(self.tmp) > 30:
            self.tmp = self.tmp[-10:]

        try:
            self.cur.execute(cmd, args)
        except sqlite3.IntegrityError as e:
            self.print(repr(e), self.path, 'key repeat')
            #self.print('\r\n' + '\r\n'.join(self.tmp), mode = 3)

    def path_insert(self, args):
        cmd = self.pathinsert
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

    def commit_line(self, line):
        db = self
        line = str(line)
        args = []
        res_t = re.search(r'(\d{4}-\d\d-\d\d \d\d:\d\d:\d\d)', line)
        num = next(self.get_num)
        if res_t is not None:
            time_s = res_t.group(1)
            time_int = int(gettimeAt(date=time_s))
        else:
            time_s = "no_time"
            time_int = num
        args.append(time_s)
        res = re.search(r'(\d+)\s+([CPUABchanel12]+)\s+(\S+):(\d+)>(.*)', line)
        if res is None:
            res = re.search(r'(\d+)\s+([CPUABchanel12]+)\s+\w+\s+(\S+):(\d+)>(.*)', line)
            if res is None:
                1#print('re search fail', lines)

        if res is not None:#若匹配成功则按格式保存
            if db.path != ':memory:':
                for i in range(1, 6):
                    args.append(res.group(i))
                args.append(time_int)
                args[1] = int(args[1])
                args[4] = int(args[4])
                #print(args)
                db.log_insert(tuple(args))
                db.path_insert(tuple(args))
        else:
            if db.path != ':memory:':
                args.append(num)
                args.append('CPU')
                args.append('print')
                args.append( 0 )
                args.append( line )
                args.append(time_int)
                db.log_insert(tuple(args))
                db.path_insert(tuple(args))

class DataBase(object):
    def __init__(self, path):
        self.path = path

    def close(self):
        self.cur.close()
        self._conn.close()
        # self.filelog.close()

    def open(self):
        if not os.path.exists(self.path):
            raise Exception()
        self._conn = sqlite3.connect(self.path)
        self.cur = self._conn.cursor()

    def commit(self):
        self._conn.commit()

def gettimeAt(date = 0, hours=0, mins=0, settime = time.time()):
    '''description:得到某个时刻之前的某个时刻的时间
    
    Attributes: date,hours,mins,settime
    
    Arguments:
      date:int or str 当参数为>=0 的int时，决定返回时间为date天前，当为负数时，返回未来的时间
                    当参数为str 时 如果格式不为"%Y-%m-%d %H:%M:%S"时抛出异常\n
      hours，mins：int 决定返回时间为多少小时、分钟前\n
      settime：决定返回时间的依据，若为空则默认为当前按时间\n
    Returns: Type: str
      返回一个UTC时间戳，单位为秒（int）
    '''
    if isinstance(date, str) == True:
        rt = datetime.strptime(date, "%Y-%m-%d %H:%M:%S")#此时输入的日期是以当前本地时区为准，返回值为utc时间戳，同样的参数在不同时区返回值不同
        rt = time.mktime(rt.timetuple())
    elif isinstance(date, int) == True: 
        if date < 0:
            rt = settime + timedelta(days = -date, hours = hours, minutes= mins).total_seconds()
        else:
            rt = settime - timedelta(days = date, hours = hours, minutes= mins).total_seconds()
    # 转换为时间戳
    #   timeStamp = int(time.mktime(threeDayAgo.timetuple()))
    return str(int(rt))

def resovleArgv(argv):
    '''处理来自命令行参数 hp:s:e:v,l:",["path=","starttime=","endtime=","savetime=","limit="]'''
    path = None

    starttime = None#创建的视图数据的起始时间，当前为100天前
    endtime = None#视图数据的终止时间，当前时4000天后
    savetime = None#若为int 路径、行号视图只包括int分钟内发送过消息的数据源（路径行号），与starttime取最近的时间
    limit = None #文件行号视图的数据量限制
    try:
        opts, args = getopt.getopt(argv, "hp:s:e:v:l:", ["path=", "starttime=", "endtime=", "savetime=", "limit="])
    except getopt.GetoptError:
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            sys.exit()
        elif opt in ("-s", "--starttime"):
            try:
                int(arg)
            except:
                pass
            else:
                arg = int(arg)
            starttime = gettimeAt(arg)
        elif opt in ("-e", "--endtime"):
            try:
                int(arg)
            except:
                pass
            else:
                arg = int(arg)
            endtime = gettimeAt(arg)
        elif opt in ("-v", "--savetime"):
            savetime = int(arg)
        elif opt in ("-p", "--path"):
            path = arg
        elif opt in ("-l", "--limit"):
            limit = int(arg)
    assert(path is not None)
    return path, starttime, endtime, savetime, limit

def bytes_decode(buf, port = '', decode='utf8'):
    '''治标不治本'''
    def on_exce(buf, decode, exec1, retry):
        if retry <= 0:
            raise exec1
        msg = str(exec1)
        res = re.search(r'decode byte 0x([\da-fA-F]+) in position', msg)
        if res is not None:
            temp = bytearray(buf)
            dst_num = int(res.group(1), 16)
            for i, t in enumerate(temp):
                if t == dst_num:
                    temp[i] = 5
                buf = bytes(temp)
            try:
                data = buf.decode()
            except exec1.__class__ as e:
                data = on_exce(buf, decode, e, retry - 1)
        else:
            raise exec1
        return data

    try:
        data = buf.decode()
    except UnicodeDecodeError as e:
        msg = str(e)
        print(msg)
        data = on_exce(buf, decode, e, len(buf))
    return data

# 将字符串按\r\n分割
def getline(data):
    '''将字符串按'\n'分割,并将最后一行与下一次的参数合并'''
    if not hasattr(getline, 'buf'):
        getline.buf = str('')
    data = str(data)
    data = re.sub('\x1b[^mABCDHJKsuIh]*[mABCDHJKsuIh]', '', getline.buf + data)#去除终端控制符
    data = re.sub('\r', '\n', data)
    data = re.sub('\n\n', '\n', data)
    lines = data.split('\n')
    getline.buf = lines.pop()#将可能未处理完成的最后一行与下一次读取的数据合并处理
    return lines
if __name__ == "__main__":
    rt_val = 0
    print(time.time())
    try:
        if len(sys.argv) > 1:#处理来自命令行的参数，若无则采用初始值
            path, starttime, endtime, savetime, limit = resovleArgv(sys.argv[1:])
        else:
            exit(1)
        #path, starttime, endtime, savetime, limit = (r"C:\Users\LiuXintian\Desktop\testplatform\Log_Log\default\1_16S.log", None,
        #                                                None, None, None)
        print(time.time())
        filename, fileType = os.path.splitext(path)
        if fileType != '.db':
            rt_val = 333
            filename = path
            path = path + '.db'
            ndb = DataBase_write(path)
            ndb.createTable1(ndb.logform)  #创建表
            ndb.createTable1(ndb.pathform)
            with io.open(filename, 'rb') as f:
                size_r = 0
                count = 0
                buf = f.read(10240)
                ndb.open() #打开数据库连接
                while len(buf) > 0:
                    size_r += len(buf)
                    dbuf = bytes_decode(buf)
                    lines = getline(dbuf)
                    for line in lines:
                        if len(line) > 0:
                            ndb.commit_line(line)
                            count += 1
                    ndb.commit()
                    buf = f.read(10240)
                ndb.close() #打开数据库连接

            print(size_r, next(ndb.get_num))
            print()
        db = DataBase(path)
        if True:
            '''log:tabble 储存所有读取到的串口数据\n
            logpath:tabble 储存读取到的最新数据\n
            V_xxxxyyy:view 来自路径xxxx行号为yyy的某段时间的视图\n
            logview:view 查看某段时间的所有数据\n
            oneview:view 查看自最新的一条记录前n 分钟的，来自不同路径行号的数据'''
            db.open()
            #清空所有视图
            db.cur.execute("select name from sqlite_master where type='view'")
            paths = db.cur.fetchall()
            for path in paths:
                cmd = "drop view if exists " + str(path).split("'")[1]
                db.cur.execute(cmd)
            db.commit()
            print(time.time())
            #创建_logview，数据时间从starttime-endtime
            if starttime is not None and endtime is not None:
                start_end = "where TIME between " + starttime +" and " + endtime
            elif starttime is not None:
                start_end = "where TIME > " + starttime
            elif endtime is not None:
                start_end = "where TIME < " + endtime
            else:
                start_end = ""

            cmd = "create view _logview as select datetime,num,cpu,path,line,msg,time from log " + start_end +" order by TIME DESC,NUM DESC"
            db.cur.execute(cmd)

            if  savetime is not None:#若定义了savetime
                savetime = int(savetime)
                cmd = 'select time from _logview order by TIME DESC,NUM DESC limit 1'#查询_logview最新的数据发送时间
                db.cur.execute(cmd)
                try:
                    settime = float(db.cur.fetchone()[0])#将settime设置为最新的数据发送时间
                except:
                    settime = time.time()
                savetime = gettimeAt(0,0,savetime,settime)#得到自settimen前n分钟的时间
                cmd = "create view _oneview as select datetime,num,cpu,path,line,msg,time from logpath where TIME > "\
                    + savetime +" order by TIME DESC,NUM DESC"
            #若未定义save time，则数据时间取starttime
            else:
                if starttime is not None:
                    cmd = "create view _oneview as select datetime,num,cpu,path,line,msg,time from logpath where TIME > "\
                        + starttime +" order by TIME DESC,NUM DESC"
                else:
                    cmd = "create view _oneview as select datetime,num,cpu,path,line,msg,time from logpath order by TIME DESC,NUM DESC"
                    
            db.cur.execute(cmd)
            db.commit()
            db.cur.execute("SELECT path,line,TIME from _oneview")
            paths = db.cur.fetchall()
            print(time.time())
            for path in paths:
                check = path[0]
                name = 'V_'+path[0] +"_"+ str(path[1])
                name = name.replace('.','_')#视图名禁止 . / 所以替换
                name = name.replace('/','_')
                cmd = "create view "+name+" as select datetime,num,cpu,msg from _logview where path = '"\
                    +check +"' and line = " + str(path[1]) + ("  limit " +str(limit) if limit is not None else "")
                db.cur.execute(cmd)
            db.commit()
            db.close()
            print('success')
            print(time.time())
            #time.sleep(5)
    except Exception as e:
        traceback.print_exc()
        rt_val = 1
        os.system('pause')
    exit(rt_val)
