        
        如果show_flag为True则打印到控制台
        如果定义了tcp则转发消息
        '''
        if self.show_flag:
            self.print(self.src_lru, self.msg)  # 打印到控制台

"""
udp_loglib 模块提供与VCU通信、日志处理及配置管理的核心实现。
其中定义了发送/接收帧结构、日志类以及测试平台 `TestPlatForm` 等。
"""
        if hasattr(Log, 'tcp'):
            Log.tcp.send(self.src_lru, self.msg)  # 转发消息到TCP客户端
    
    @classmethod
    @funclog
    def path_set(cls, path):
        '''设置日志保存路径
        
        参数:
          path: 新的日志保存路径
        '''
        with threading.Lock():  # 线程安全操作
            cls._root = os.path.abspath(path) + '/'  # 设置路径
            if not os.path.exists(cls._root):
                try:
                    os.makedirs(cls._root)  # 创建目录
                except Exception:
                    pass
        mPrint('Log path set to %s' % cls._root)
        funclog.sub_filepath = cls._root + 'funclog.log'
        
    @classmethod
    def path_get(cls):
        '''获取当前日志保存路径
        
        返回:
          当前日志保存的根路径
        '''
        return cls._root 

    def _store(self, msg=''):
        '''存储日志消息到文件
        
        参数:
          msg: 要存储的消息，默认为self.msg
        '''
        dst = self.path_get() + self.src_lru + '.log'
        if len(msg) == 0:
            msg = self.msg
            dbcommit(self.db_get(self.src_lru), self.msg)
        if not os.path.exists(os.path.dirname(dst)):
            os.makedirs(os.path.dirname(dst))  # 批量创建目录
        with open(dst, 'a+') as f:
            f.write(VCU.gettime() + ' ' + self.src_lru + ' ' + msg + '\r\n')

class Reply(Default):
    '''回复消息类，处理确认回复消息
    
    注意: Reply目前未作为常规消息，在UDP接收处特殊处理
    '''
    _struct = [('tstData', 1, '')]  # 结构体定义，参考UdpFrame.parse_data方法
    _mtype = '3X'  # 消息类型标识(不会用到)
    _toRun_list = []  # 执行操作列表(不会用到)
    
    def __init__(self):
        '''初始化回复消息对象'''
        pass
        
    @classmethod
    @funclog
    def _reply_Send(cls, string):
        '''生成回复消息
        
        参数:
          string: 原始消息字符串
          
        返回:
          回复消息字符串
        '''
        frame = UdpRecvFrame(string, cls)  # 解析源消息

        srcNodeID = 'FF'  # 本机节点ID
        destNodeID = frame.srcNodeID  # 目标节点ID为源消息的源节点
        
        # A类型消息需要回复，回复时消息类型第一位改为3表示回复
        dataType = list(frame.dataType)  
        dataType[0] = '3'  # 第一位设为3表示回复
        dataType = ''.join(dataType)
        
        dataLen = frame.dataLen  # 长度与源消息相同
        tstData = cls.tstData  # 消息内容不变，校验码不重新计算
        
        # 组合回复消息
        data = destNodeID + srcNodeID + dataType + dataLen + tstData
        return data

    @classmethod
    @funclog
    def _reply_Recv(cls, string):
        '''处理收到的回复消息
        
        参数:
          string: 回复消息字符串
        '''
        frame = UdpRecvFrame(string, cls)  # 解析回复消息
        
        # 提取关键信息
        flag = frame.srcNodeID + frame.dataType[1] + cls.tstData  # 源节点ID + 消息类型第二位 + 内容
        
        # 添加到回复列表
        FrameLib.respond_list.append((flag.upper(), time.time()))
        
        # 清理过期的回复记录
        if len(FrameLib.respond_list) > 0 and time.time() - FrameLib.respond_list[0][1] > 50.0:
            FrameLib.respond_list.pop(0)
            mPrint('_reply_Recv', time.time() - FrameLib.respond_list[0][1])

    def _default_run(self, frame):
        '''处理接收到的回复消息
        
        参数:
          frame: 回复帧对象
        '''
        Reply.tstData = self.tstData  # 保存消息内容
        # 过滤终端控制字符，处理换行符
        self.info = re.sub('\x1b[^mABCDHJKsuIh]*[mABCDHJKsuIh]', '',
                           self.info).replace('\n', '\\n').replace('\r', '')

        # 添加到回复列表
        FrameLib.respond_list.append((flag.upper(), time.time()))
        
        # 清理过期的回复记录
        if len(FrameLib.respond_list) > 0 and time.time() - FrameLib.respond_list[0][1] > 50.0:
            FrameLib.respond_list.pop(0)
            mPrint('_reply_Recv', time.time() - FrameLib.respond_list[0][1])

#endregion

class TestPlatForm(object):
    @funclog
    def __init__(self, srcPort: int, dstPort: int, set_client = True):
        '''description: 发送测试命令的基本类
        Attributes: 
          lru_auto: bool 若为 True 发送l指令时会自动过滤不可用槽道
          Serial_Flag: bool 若为True 则会自动保存可用的串口消息
          mib: Mib  可通过此属性获取mib 信息 rt = self.mib.get_value('LruDefaultCode'， outfile = None)
          wireShark: Wireshark self.wireshark.start(...) 开始抓包
          update: func 在线更新镜像的函数
          update_cfg: func 在线更新配置的函数
          vmtool: VMTool 与虚拟机交互的实例
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          srcPort:int 本地端口号
          dstPort:int VCU 端口号
          set_client： 自动设置udp client 默认为[]
        Returns: Type:None
          无返回值
        '''
   
        myAssert(not hasattr(TestPlatForm, 'form') )
        gevent.spawn(gevent.empty)  #空协程，统计空闲时间
        self.__srcPort = srcPort
        self.__dstPort = dstPort
        self.clientS = UdpClient((Config.local_ip[16], srcPort),
                                 (Config.slot_ip[16], dstPort))  #备系非安全网
        self.clientP = UdpClient((Config.local_ip[2], srcPort),
                                 (Config.slot_ip[2], dstPort))  #主系非安全网
        if set_client:
            self._set_client(mode = [], retry=0)
        TestPlatForm.form = self

        self._lru_auto = False
        self.Serial_Flag = False
        self.mib = Mib('ICMTC', Config.slot_ip, print_func = mPrint, file_root_get = Log.path_get)
        self.wireShark = Wireshark(Config.wireshark_path, Config.default_ethdesc, print_func = mPrint, file_root_get = Log.path_get)
        self._SerialRead = SerialRead(print_func = mPrint, file_root_get = Log.path_get)
        rut.slot_ip = Config.slot_ip
        #self.update = rut.update
        self.update_cfg = rut.update_cfg
        self.vmtool = VMTool()
        self.vmtool.rut = rut
        self.__viv_dict = {}
        self.__svcu_set = set()
        self.__dump_flag = (False, time.time())

    def update(self, fileType, act, module_3, dirname = '', block = True, retry = 3, savepath=None):
        if Config.version.startswith('102') and act == 0:
            act = 6
        elif act == 6:
            act = 0

        print("lead in udp_logic.update")
        return rut.update(fileType, act, module_3, dirname, block, retry, savepath)

    @funclog
    def get_dump(self, moudle_3, CPU = 'AB', outfile_dir = None):
        if outfile_dir is None:
            outfile_dir = Log.path_get()
        assert(os.path.isdir(outfile_dir))
        rut.update_dump(moudle_3, CPU, dirname=outfile_dir)

    @funclog
    def get_eventlog(self, moudle_3):
        '''获取 vcu eventlog 结果自动保存在'EventLog' 文件夹
            moudle_3: str|list 目标槽道
        '''
        if not isinstance(moudle_3, list):
            moudle_3 = VCU.inputsplit(moudle_3)
        dirname = Log.path_get() + '\\EventLog'
        if not os.path.isdir(dirname):
            os.mkdir(dirname)

        for slot in moudle_3:
            viv = VCU_VIV(slot)
            for cpu in ['A', 'B']:
                event = viv.get_eventlog(cpu)
                dstpath = dirname + '\\%s_%s.log' % (VCU.node2lru((slot + 3) * 2)[0], cpu)
                if len(event) > 20:
                    mode = 'w'
                else:
                    mode = 'a'
                    if os.path.isfile(dstpath):
                        event.insert(0, '\r\nSome items followed may repeat')
                with open(dstpath, mode) as f:
                    f.write('\r\n'.join(event))

    #@staticmethod
    def __get_funcMode(self, module_3: int):
        try:
            viv = VCU_VIV(module_3)
        except Public.myException:
            funcMode = -1
        else:
            try:
                funcMode = viv.get_functionmode()
            except TimeoutError:
                funcMode = -1 #获取失败则返回
        return module_3, funcMode

    @funclog       
    def get_funcMode(self, slots) -> dict:
        '''description: 获取功能模式
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          slots: str|list 所需获取的槽道号
        Returns: Type:dict
          dict[slot] = funcmode
        '''
        from trys import Trys

        '''通过viv 获取功能模式， vcu boot -> limation ->normal'''
        funcMode_dict = {0: 'NORMAL', 1: 'NORMAL', 2: 'NORMAL', 3: 'LIMATION', 4: 'LIMATION', 5: 'LIMATION', 6: 'BOOT_FAILED', 7: 'REPROG', 8: 'BOOT', -1: 'VIV_ERROR'}

        if not isinstance(slots, list):
            slots = VCU.inputsplit(slots)
        #trys = Trys(todo=slots, target='module_3', rt_type='all')
        func_modes = []
        #func_modes = trys(self.__get_funcMode)()
        for module_3 in slots:
            func_modes.append( self.__get_funcMode(module_3))

        rt_dict = {}
        for module_3, funcMode in func_modes:
            rt_dict[int(module_3)] = funcMode_dict[funcMode]
        
        return rt_dict

    @funclog
    def get_active_mvcu(self) -> int:
        '''获取主mvcu 的槽道号'''
        from trys import Trys
        funcMode_dict = {0: 'MVCUA', 3: 'MVCUA', 1: 'MVCUS', 4: 'MVCUS'}

        rt = []
        trys = Trys(todo=[2, 16], target='module_3', rt_type='all')
        func_modes = trys(self.__get_funcMode)()
        for module_3, funcMode in func_modes:
            if funcMode_dict.get(funcMode, 'MVCUSORERROR') == 'MVCUA':
                rt.append(module_3)

        if len(rt) > 1:
            raise Exception(rt)
        elif len(rt) == 0:
            return None
        else:
            return rt[0]

    @funclog
    def get_standby_mvcu(self) -> int:
        '''获取备mvcu 的槽道号'''
        from trys import Trys
        funcMode_dict = {0: 'MVCUA', 3: 'MVCUA', 1: 'MVCUS', 4: 'MVCUS'}

        rt = []
        trys = Trys(todo=[2, 16], target='module_3', rt_type='all')
        func_modes = trys(self.__get_funcMode)()
        for module_3, funcMode in func_modes:
            if funcMode_dict.get(funcMode, 'MVCUSORERROR') == 'MVCUS':
                rt.append(module_3)

        if len(rt) > 1:
            raise Exception(rt)
        elif len(rt) == 0:
            return None
        else:
            return rt[0]

    @property
    def lru_auto(self):
        '''为True 后 会自动过滤掉不可用的槽道，但因mib 信息存在延迟，可能会不准，耗时根据配置，最大额外耗时为7秒左右'''
        return self._lru_auto

    @lru_auto.setter
    def lru_auto(self, val: bool):
        '''为True 后 会自动过滤掉不可用的槽道，但因mib 信息存在延迟，可能会不准，耗时根据配置，最大额外耗时为7秒左右'''
        self._lru_auto = val

    @funclog
    def _VCUs_available(self, moudle_3s: list) -> list:
        '''description:获取可用的槽道号        
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          moudle_3s：list 原始list 
        Returns: Type:list
          返回原始list 中可用的 槽道号 
        '''
        self._avais = self.mib.VCUs_available(moudle_3s)

        moudle_3s_temp = [m for m in moudle_3s if m not in self._avais]
        if len(moudle_3s_temp) > 0:
            mPrint('these lrus not available', moudle_3s_temp)
        return self._avais

    @funclog
    def tst_set(self, slots: str, frame: UdpSendFrame, openflag=True, autowait = True) -> None:
        '''description:向VCU发送指令
        
        Attributes: self, slot, func, frame, openflag, autowait
        
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          slot:str 格式为'3-8,2.16，9'，-表示连续，[,，.]表示离散，数字为moudleID - 3
          frame:UdpSendFrame 
          openflag:bool 如果为真且该 func 被允许，将自动打开allFlag，默认为TRUE，无需显式赋值
          autowait:nool 如果为真则命令发送后会自动调用FrameLib.respondCheck() （检查命令是否发送成功）
        Returns: Type:None
          无返回值
        '''

        if frame.mtype in [
                Tag
        ] and openflag is True:  #若flag 为True 会默认为允许的类型打开 udp 和 all 标志
            self.tst_set(slots, Conf.allFlagSet(1), False, False)

        if isinstance(slots, list):
            moudle_3s = slots
        else:
            moudle_3s = VCU.inputsplit(slots)

        if self.lru_auto:  # 将此标志位置为true 时 将会自动根据mib的信息过滤槽道号，但mib信息可能延迟
            moudle_3s = self._VCUs_available(moudle_3s)

        frame.frameSendCallBack(moudle_3s)

        svcus = [slot for slot in moudle_3s if slot != 16 and slot != 2]
        if self.__dump_flag[0] and time.time() - self.__dump_flag[1] < 60:
            self.__dump_flag = (False, time.time())
            frame.isRecord = False
        elif frame.isRecord:
            for i in svcus:
                self.__svcu_set.add(i) # self.__svcu_set
        
        for moudle_3 in set(moudle_3s):
            node1, node2 = VCU.moudle_3toNode(moudle_3)  #cpuA,B
            frame.srcNodeID = 'FF'
            frame.dstNodeID = node1
            if frame.mtype._mtype[0] == '1':  #若需要回复，一段时间未收到回复则重发
                frame.crc = '0000'  # 表示crc 使用crc16
                frame.crc = Public.crc16(frame.data,
                                         int(frame.len, 16) - len(frame.crc) // 2)  # crc 不参与crc计算
                FrameLib.resigeter(frame.copy())
            if (int(frame.dstNodeID, 16) // 2) % 2 == 0: # moudleid 为偶数，则槽道号为奇数
                self._client_even.send(frame)  #通过1——16
            else:
                self._client_odd.send(frame) #通过1——2
        if autowait:
            FrameLib.respondCheck()

    @funclog
    def before_dump(self, slots):
        '''在板卡宕机前调用，'''
        self.tst_set(slots, Conf.notReprogOnDump(), openflag = False, autowait=False) # 临时关闭dump 破坏reporg
        self.tst_set(slots, Conf.notBurnFuse(), openflag = False, autowait=False) # 临时关闭dump 破坏reporg
        if isinstance(slots, list):
            moudle_3s = slots
        else:
            moudle_3s = VCU.inputsplit(slots)
        moudle_3s = set(moudle_3s) # 避免两次重复的mvcu  
        mvcus = [ slot for slot in moudle_3s if slot == 2 or slot == 16]

        if len(mvcus) == 1: # 长度为0表示双系均不宕机，无需切换， 长度为2 表示双系均会宕机， 无需切换 
            if 2 in moudle_3s: # dst = moudle_3s[0]
                dst = 16
            else:
                dst = 2
            try:
                self._set_client(dst, dst_svcu=list(self.__svcu_set), retry = 1) # 切换主mvcu
            except Public.myException:
                mPrint('the other mvcu is not usable!')
        
        svcus = moudle_3s ^ set(mvcus)
        self.__svcu_set ^= svcus & self.__svcu_set # 删除之前的消息的记录
        self.__dump_flag = (True, time.time())
        FrameLib.respondCheck()
   
    @funclog
    def _log_recv(self, client: UdpClient):
        """从指定UDP客户端持续接收日志并解析。"""
        mPrint('recv start %s,%s' % client.remote)
        Public.thread_flag['recv'] = Public.thread_flag.get('recv', True)
        while Public.thread_flag['recv']:
            data = client.recv()
            if data is not None:
                frame = UdpRecvFrame(data)
                #默认vcu只会主动往外发log 消息，即frame.data_type始终指向Log
                frame.data_type._run_list()
                gevent.sleep(0)
            else:
                gevent.sleep(0.01)    
                 
    @funclog
    def _reSend(self, frame: UdpSendFrame):
        """在未收到应答时重新发送指定帧。"""
        dst = frame.dstNodeID
        frame.isRecord = frame.isRecord # 重注册
        FrameLib.resigeter(frame)  # 重注册
        dst = int(dst, 16)
        if (int(frame.dstNodeID, 16) // 2) % 2 == 0: 
            self._client_even.send(frame)  #发送给CPUA
        else:
            self._client_odd.send(frame)

    def _serial_recv(self):
        """监听串口日志并转为统一的日志对象。"""
        Public.thread_flag['serial_recv'] = Public.thread_flag.get('serial_recv', True)
        flag = True
        while Public.thread_flag['serial_recv']:
            if self.Serial_Flag:
                if flag:
                    self._SerialRead.open_all(outdirname = None)
                    flag = False
                    mPrint('start listen', self._SerialRead.queue_dict.keys())
                for port, que in self._SerialRead.queue_dict.items():
                    try:
                        l_msg = que.get(block = False)
                    except Exception:
                        g_sleep(0.2)
                        continue
                    
                    serial_log = Log()
                    serial_log.src_lru = port + '_' #TODO 不加_ 特殊情况下会出现异常
                    serial_log.msg = l_msg
                    res = re.search(r'(\d+)\s+([CPUABchanel12]+)\s+\w+\s+(\S+):(\d+)>(.*)', l_msg)
                    if res is not None:
                        serial_log.info = res.group(5)
                        src_cpu = res.group(2)
                        __dict = {'chanel1': 'CPUA', 'chanel2': 'CPUB'}
                        serial_log.src_cpu = __dict.get(src_cpu, src_cpu)
                        serial_log.recvVal = []
                    else:
                        serial_log.info = l_msg
                        serial_log.src_cpu = 'CPUA'
                        serial_log.recvVal = [] #TODO 该功能只有udp_report 函数打印出来有用， 待改进 暂时未用到 
                    
                    serial_log._run_list()

                for g in self._SerialRead.greenlet_list:
                    if g.dead:
                        try:
                            g.get()
                        except Exception as e:
                            raise e
                gevent.sleep(0)
            else:
                if flag is False:
                    self._SerialRead.close_all()
                    flag = True
                gevent.sleep(1)
        self._SerialRead.close_all()

    def _wireshark_recv(self):
        """周期性抓取网络报文并保存到当前日志目录。"""
        Public.thread_flag['_wireshark_recv'] = Public.thread_flag.get('_wireshark_recv', True)
        temp_path = ''
        p_new = None

        def taskkill_dumpcap():
            p = subprocess.Popen('taskkill /t /f /fi "WINDOWTITLE eq %s\\test_dumpcap.exe"' % Config.wireshark_path, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            out = p.stdout.read()
            out = out.decode('gbk')

            # if '没有运行的带有指定标准的任务' not in out:
            #     mPrint(out)

        def kill_p_old(p_old):
            g_sleep(5)
            p_old.kill()

        taskkill_dumpcap()
        Public._end_process.append((taskkill_dumpcap, ()))
        while Public.thread_flag['_wireshark_recv']:
            if temp_path != Log.path_get():
                temp_path = Log.path_get()
                p_old = p_new
                p_new = self.wireShark.start(outfile='wireshark_recv.pcapng', block=False)
                if p_old is not None:
                    gevent.spawn(kill_p_old, p_old)
            else:
                g_sleep(1)

    def _mib_recv(self):
        """定时获取MIB信息，用于判断槽位状态。"""
        Public.thread_flag['_mib_recv'] = Public.thread_flag.get('_mib_recv', True)
        temp_path = ''
        count = 0
        while Public.thread_flag['_mib_recv']:
            if temp_path != Log.path_get():
                temp_path = Log.path_get()
                count = 100
            else:
                count += 1
                g_sleep(1)   
            if count > 60: # 每分钟获取一次
                self.mib.get_value('lruDefaultCode', outfile= 'auto_getmib.log', raise_error=False)
                count = 0

    def _set_client(self, mode = [], dst_svcu = None, retry = 3):
        '''description: 指定发送消息的MVCU，当使用两个mvcu 时 发送给奇数槽道的svcu的消息会通过 mvcuS 转发， 发送给偶数svcu 的槽道的消息会通过mvcuP 转发
        
        Attributes: None
        
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          mode:str|list 格式为 '2' '16' '2,16' [2,16] [] 若为空则自动选择
          retry:int 重新连接次数， 若无需重连则设为0
        Returns: Type:None
          无返回值
        '''
        if hasattr(self, '_active_svcu') and dst_svcu is None:
            dst_svcu = self._active_svcu

        if not isinstance(mode, list):
            mode = VCU.inputsplit(mode)

        for i in range(len(mode)):
            mode[i] = int(mode[i])
        if retry < 0:
            raise Public.myException( mode, 'ip is not reachable')

        __client = None
        _client_odd = None
        _client_even = None
        if Public.isIPreachable(Config.slot_ip[2]):  #判断可用的mvcu
            __client = self.clientP
            _client_odd = __client
            
        if Public.isIPreachable(Config.slot_ip[16]):
            __client = self.clientS
            _client_even = __client

        if __client is None:
            return self._set_client(mode, retry=retry - 1)

        if (2 in mode and 16 in mode): # 指定两个mvcu 同时工作
            if _client_odd is None or _client_even is None:
                gevent.sleep(5)
                return self._set_client(mode, retry=retry - 1)
        elif 16 not in mode and 2 not in mode: # 没有指定任何mvcu
            if _client_odd is None:
                _client_odd = _client_even
            elif _client_even is None:
                _client_even = _client_odd
        elif 16 in mode: # 指定16
            if _client_even is None:
                gevent.sleep(5)
                return self._set_client(mode, retry=retry - 1)
            else:
                _client_odd = _client_even
        else:# 指定2
            if _client_odd is None:
                gevent.sleep(5)
                return self._set_client(mode, retry=retry - 1)
            else:
                _client_even = _client_odd
                
        if _client_odd is _client_even:
            if _client_odd is self.clientP:
                mPrint('use mvcu p')
            else:
                mPrint('use mvcu S')
        else:
            mPrint('use 2 mvcu')
        self._client_odd = _client_odd
        self._client_even = _client_even 
        if dst_svcu is not None and len(dst_svcu) > 0:
            self.tst_set(dst_svcu, Conf.serialFlagSet(1))           

def gevent_join(greenlet_list):
    '''启动协程'''
    form = TestPlatForm.form

    if not isinstance(greenlet_list, list):
        greenlet_list = [gevent.spawn(greenlet_list)]
    elif len(greenlet_list) > 0:
        if not isinstance(greenlet_list[0], greenlet.Greenlet):
            for i in range(len(greenlet_list)):
                greenlet_list[i] = gevent.spawn(greenlet_list[i])

    default_list = [
        gevent.spawn(form._log_recv, *(form.clientP, )),  # 接收1_2P
        gevent.spawn(form._log_recv, *(form.clientS, )),  # 接收1_16S
        gevent.spawn(form._serial_recv),
        gevent.spawn(form._mib_recv),
        gevent.spawn(form._wireshark_recv)]

    default_list += greenlet_list
    gevent.joinall(default_list)

if __name__ == "__main__":
    viv = VCU_VIV(8)
    viv._cop_open('A')
    viv._SessionOpen('A')
    viv._MemReadInit('A', data='0001000000004000', dst = 'SPI')
    viv_list = []
    for i in range(50):
        temp = VCU_VIV(8)
        temp._o_data = viv._o_data
        temp._copopen_dict = viv._copopen_dict
        viv_list.append(temp)
    g_list = []
    for i in range(8):
        g = threading.Thread(targert = viv_list[i]._cop_open, args=('A',))
        g_list.append(g)
    time.sleep(10)
    viv._cop_close('A')
class Conf(Default):
    '''配置消息类，用于设备配置
    
    属性:
      _struct: 结构体定义
      _mtype: 消息类型
      _toRun_list: 要执行的方法列表
      _lru: LRU文件路径
    '''
    _struct = [('cmdCode', 2, ''),  # 命令代码
               ('para1', 4, ''),    # 参数1
               ('para2', 4, ''),    # 参数2
               ('para3', 4, ''),    # 参数3
               ('para4', 4, ''),    # 参数4
               ('para5', 4, ''),    # 参数5
               ('para6', 4, ''),    # 参数6
               ('para7', 4, ''),    # 参数7
               ('para8', 4, ''),    # 参数8
               ('para9', 4, '')]    # 参数9
    _mtype = 'A6'  # 消息类型
    _toRun_list = ['_conf_run', '_default_run']  # 执行方法列表
    _lru = cfg_path()
    
    @classmethod
    def getmode(cls, lru=''):
        '''获取LRU的测试模式
        
        参数:
          lru: LRU标识符，默认为空
          
        返回:
          测试模式设置
        '''
        if not lru:
            return
        # 检查是否存在配置文件
        dst = cls._lru + lru + '.json'
        if not os.path.exists(dst):
            return "STD"  # 默认返回STD模式

        try:
            # 读取配置文件内容
            with open(dst) as f:
                dic = json.load(f)
            return dic.get('Mode', 'STD')  # 返回模式，默认为STD
        except Exception as e:
            mPrint(f'Conf.getmode error: {e}')
            return "STD"  # 出错时返回STD

    @classmethod
    def setmode(cls, lru, mode):
        '''设置LRU的测试模式
        
        参数:
          lru: LRU标识符
          mode: 要设置的模式
        '''
        if not lru:
            return
        # 检查是否存在配置文件
        dst = cls._lru + lru + '.json'
        dic = {}
        
        if os.path.exists(dst):
            try:
                # 读取现有配置
                with open(dst) as f:
                    dic = json.load(f)
            except Exception as e:
                mPrint(f'Conf.setmode load error: {e}')
        
        # 更新模式设置
        dic['Mode'] = mode
        
        try:
            # 确保目录存在
            if not os.path.exists(os.path.dirname(dst)):
                os.makedirs(os.path.dirname(dst))
            # 写入配置文件
            with open(dst, 'w') as f:
                json.dump(dic, f)
        except Exception as e:
            mPrint(f'Conf.setmode write error: {e}')
    
    def _conf_run(self, frame):
        '''配置处理方法
        
        参数:
          frame: 配置帧对象
        '''
        from VCU.src.model_list import model_list
        
        # 组合LRU标识符
        lru = frame.destNodeID + frame.srcNodeID + frame.dataType[1]
        
        # 检查命令代码
        if self.cmdCode == '0001': 
            # 切换模式命令
            value = self.para1.strip('0')
            # 检查模式是否有效
            if value not in model_list:
                return
            # 设置模式
            self.setmode(lru, value)
            return
            
        if self.cmdCode == '0002':
            # 读取模式命令
            result = self.getmode(lru)
            MsgCtrller.tx_Msg(lru, f'Mode: {result}')
            return
