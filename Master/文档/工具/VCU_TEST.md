# VCU_TEST

> 20211102

## 一.  功能

### 1. 桩打开/关闭

   测试平台与VCU 通过UDP 通信来控制桩的打开与关闭

   宏定义桩参考 “控制虚拟机”

   首先通过TestPlatForm 类初始化用来发送命令的实例

```python
    form = TestPlatForm(18125, 18125)
```

    TestPlatForm 类初始化需要两个变量，分别是本地，VCU 的 测试平台的端口号，一般为 18125， ip 地址则需在 Config 类中指定
    初始化的实例 一般都命名为form

```python
#打开桩
    form.tst_set('2,3.5,7-14', Tag.tag_send('00010111')) 
#关闭桩
    form.tst_set('3-8', Tag.tag_send('000101' + '00'))
'''
    Tag 是控制桩的类，tag_send 是 生成 开关桩的指令，并通过tst_set 发送 
'''
```

   tst_set 是 向 VCU 的 udp 发送命令的方法，该方法有 4 个参数

```python
def tst_set(self, slot, frame, openflag=True, autowait = True):
    '''description:向VCU发送指令

    Arguments:
      self:instance 调用该函数的实例，无需手动赋值
      slot:str 格式为'3-8,2.16，9'，-表示连续，[,，.]表示离散，数字为moudleID - 3
      frame:UdpSendFrame 由Tag or Conf 类的方法的返回值
      openflag:bool 如果为真且该 func 被允许，将自动打开allFlag，默认为TRUE，无需显式赋值
      autowait:nool 如果为真则命令发送后会自动调用FrameLib.respondCheck() （检查命令是否发送成功）
    Returns: Type:None
      无返回值
    '''
```

### 2. 接收 UDP LOG

    VCU 通过UDP 发送的LOG 默认会发送到20.2.1.100 或 20.1.1.100 的 18125端口。若VCU收到来自测试平台的消息，则会将ip 地址更改为所接收到的报文的源地址，端口号不会被更改。测试平台会默认同时接收两个MVCU的网络消息，无需主动调用。

   Log 保存的目录可通过 Log.path_set() 指定，文件名 如 ‘1_2P.log'

接收到的消息默认会向数据库中保存一份，保存路径固定为LOG_LOG/database

### 3. 网络抓包

  测试平台在开始运行时主动保存网络的报文，保存在Log.path_set() 指定的路径，文件名为 wireshark_recv.pcapng

  可主动调用

```python
form.wireShark.start(outfile, block=False)
def start(self,
              count=None,
              timeout=None,
              outfile=None,
              fifter=None,
              block=True):
        '''
            count:int 捕捉到 count 条报文后停止
            timeout:int 一次start 最大持续 timeout s
            outfile:str 报文保存路径
            fifter:str 过滤，和wireshark 格式不同 比如：“udp src port not 50000 and udp src port not 50001”
            block:boolean 是否阻塞
        '''
```

### 4. MIB 数据获取

   测试平台在开始运行时每间隔一分保存通过MIb 获取 的 lruDefaultCode，保存在Log.path_set() 指定的路径，文件名为 auto_getmib.log，当保存路径被更改时会立刻获取一次

    可通过form.mib 获取更多信息,  获取时间和配置有关，最大不超过7s

```python
form.mib.get_value('lruDefaultCode', outfile = 'mib.log')
def get_value(self, name, lru = 0, outfile = None, raise_error = True):
        '''description:获取MIB 消息

        Attributes: self, name, lru, outfile, raise_error

        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          name:str 目标属性名称
          lru:int  仅可当 name 是 Entry 有效，使mib 仅获取该lru的信息
          outfile:str  当不为 None 时，会将结果以追加的方式保存，无需指定目录 
          raise_error:bool 如果为真则获取mib 失败时会抛出异常
        Returns: Type:str or list
          当获取的 mib 为 entry 且 lru 为 0 时 返回 list，否则为str
        '''
```

### 5. 接收串口LOG

  若 form.serial_Flag is True, 则会自动保存所有可用串口的log，保存在Log.path_set() 指定的路径，文件名为 comN.log, 此标志为True 时不能打开任何操作打印串口的工具。若拔出串口， 否则会停止读取此串口

### 6. 分析 LOG

  UDP 和 串口 的LOG 都会被额外存放在LOG_LOG/database 内，可通过updateview对其进行分析， 每个不同来源的消息会被存放在不同的视图内，并可指定所需查询的日期范围。

```batch
rem  -s starttime #创建的视图数据的起始时间
rem  -e endtime  #视图数据的终止时间，-1
rem  这两个参数为整形时，正时为当前时间多少天前，为负时为多少天后
rem  这两个参数为%Y-%m-%d %H:%M:%S 形式时，以该形式为准
rem  -v savetime #路径、行号视图只包括savetime 分钟内发送过消息的数据源（路径行号），与starttime取最近的时间,r若未定义则取starttime
rem   -l limit = 10000 #文件行号视图的数据量限制
python %~dp0updateview.py -p %1 -s 5 -e -10 -v 10000 -l 10000
start %~dp0SQLiteSpy/SQLiteSpy.exe %1
```

 ![](.\pictures\2021-10-21-13-27-53-image.png)

   打开wireshark 会 自动检测出 RSSP1 或 FSFB2 或 测试平台的 消息类型，并解析字段

  ![](.\pictures\2021-10-27-09-57-29-image.png)

### 7. 控制虚拟机

 测试平台 与 虚拟机通过tcp 进行通信

```python
# cmd_type 为 ’svcu' 或 'mvcu'
#从PC 将源代码复制到虚拟机
form.vmtool.copySrc_toVM(cmd_type) 
# 打开两个源代码的宏定义桩，且不在此列表的宏定义桩会被关闭
# tst_vcu_stubs.h 文件内不能有重名的宏（即使被注释掉）
form.vmtool.openTagInit(cmd_type, ['VCU_MVCU_SwRTC_5201004','VCU_MVCU_SwRTC_5201005'])
# 编译文件
form.vmtool.build(cmd_type, block = True)
# 将镜像拷贝至PC
form.vmtool.copyImage_fromVM(cmd_type，dstpath=None) #dstpath 为None 取决于虚拟机的脚本配置
```

通过将以上命令合并得到一个方便的脚本：

```batch
rem 槽道分隔符仅支持 . ,
rem arg1:等价于 copySrc_toVM openTagInit build copyImage_fromVM update
rem arg2:将目标槽道识别为svcu 或 mvcu 并进行编译，随后自动更新, 不支持mvcu, svcu 同时输入
rem arg3:若未输入则不打开初始化桩
rem 若无arg, 则需手动输入槽道号，不能打开初始化桩
python3 vmtcp.py copy_build 3.4.5.6.7.8 VCU_MVCU_SwRTC_5200001，VCU_MVCU_SwRTC_5200002
```

### 8. 自动执行与脚本指导

   首先完成caseFunc.py 内的配置， 且准备好自动执行的脚本，**运行case_test/main.py**

   测试脚本以**case_test/testcase/default.py 为模板**， 自动执行仅会执行main 函数

   测试脚本需按照Cfg2\_SyRTC\_0001.py, Cfg_SwITC_SVCU_0002.ad 格式命名

   Cfgn 为本次测试要求所需的配置序号，若无配置要求输入Cfg。

   测试脚本的log 会被保存在脚本所在目录下的Log/name 文件夹

   测试脚本出现异常时其他的脚本会继续正常执行，执行记录存放在testplatform\case_test\log

   **自动测试前需保证当前VCU 及虚拟机对应配置的镜像是可用的**，开始时会将当前虚拟机的镜像做备份，用例切换时VCU 会切换至此镜像

  自动执行的脚本支持单个运行与自动运行，测试平台初始化完成后会执行 pre__main 函数

 然后执行main。

```python
def LogPath_set(path):
    '''设置log 保存路径， 以脚本当前目录所造的log文件夹为根目录'''
    dir_path = os.path.dirname(__file__) + '/Log/%s/%s' % (os.path.splitext(os.path.basename(__file__))[0], path)
    Log.path_set(dir_path)

def get_svcu() -> list:
    '''获取当前caseFunc.Config.active_svcu 所设置的svcu'''
    svcus = caseFunc.Config.active_svcu()
    svcus = caseFunc.VCU.inputsplit(svcus)
    return svcus

def before_main():
    LogPath_set('default')
'''''''''''' # 以上为固定头
requirement = caseFunc.StubRequirement(slots=None, svcu_count=None) #slots='2,3-8'  # 当前用例的```python
#requirement = caseFunc.StubRequirement(slots='2.16, 5', svcu_count=5) #slots='2,3-8'  # 当前用例的环境需求 ，该变量必须赋值
# 该行自动执行前会检查caseFunc.Config是否配置了2.16.5，caseFunc.Config 配置了 svcu 
#数量至少为5
def main():
    pass # 测试脚本入口
```

  脚本若不限制svcu 具体槽道，建议以get_svcu() 函数获取 ,可通过该列表获取指定数量的svcu

  可通过form.get_active_mvcu() 获取主mvcu form.get_standby_mvcu() 获取备系mvcu

### 9. 与VCU 通信并实现自定义功能

 通过Conf 类 （VCU 指定部分参数和调用部分函数）

 目前可用的配置项为

```python
 #更改VCU的打印flag
 form.tst_set('3', Conf.allFlagSet(1))
 #更改VCU的打印过滤周期
 form.tst_set('3', Conf.bothCycleSet(20))
 #让VCU宕机
 form.tst_set('3', Conf.shutdown())
 #主备切换
 form.tst_set('2', Conf.tostandby())
 #触发错误过滤
 form.tst_set('2', Conf.TriggerFaultFifter()) # 确保gdf.c 文件已经加入测试平台
 
 form.tst_set('3', Conf.sendOrRecvCount(2，3)) # 仅svcu 可用， 确保guest.c 加入此功能， 同时
                                               #GUEST_F_Start 函数内初始化两个数组的size 改为128
 #允许错误过滤
 form.tst_set('16', Conf.AllowFaultFifter()) # 确保gdf.c 文件已经加入测试平台
 
 #宕机不烧熔丝  仅宏VCU_BURN_FUSE_DISABLE 未定义时可用
 form.tst_set('16', Conf.notBurnFuse()) # 确保gdf.c 文件已经加入测试平台，
 
 #宕机不进入在线更新模式  仅宏VCU_BURN_FUSE_DISABLE 已定义 且宏ReprogBreakOnDump 已定义时可用， 
 form.tst_set('16', Conf.notReprogOnDump()) # 确保gdf.c 文件已经加入测试平台
 
 #获取功能模式（通过viv 的 -i 命令）
 func_mode_dict = form.get_funcMode('3-8')
     def get_funcMode(self, slots) -> dict:
        '''description: 获取功能模式
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          slots: str|list 所需获取的槽道号
        Returns: Type:dict
          dict[slot] = funcmode
     '''
 #获取主备
 active_mvcu = form.get_active_mvcu()
 standby = form.get_standby_mvcu() 
     def get_active_mvcu(self) -> int:
        '''获取主mvcu 的槽道号'''
 #获取mib 见MIB 数据获取
 #获取eventlog
  form.get_eventlog('2-14,16')
     def get_eventlog(self, moudle_3):
        '''获取 vcu eventlog 结果自动保存在‘EventLog’ 文件夹
            moudle_3: str|list 目标槽道
        '''
```

### 10. 与模拟器通信：

  测试平台不能通过网络与模拟器通信，但可通过修改初始化的值更改模拟器发送的数据。

  修改后的模拟器提供了一个initValue.ini 文件， 通过修改对应的value可设置模拟器的初值

  initValue.ini

```ini
RSSP1-1:E000
RSSP1-2:0000
RSSP1-3:0000
RSSP1-4:0000
RSSP1-5:E000
RSSP1-6:E000
```

格式为name:value, 每一行的空格会被忽略

name 与模拟器的version.ini 的 VERSION 一致。

value 是16进制的值， E000 表示模拟器前三个码位设置为一，其他码位设置为0

value 最长为56位，对应模拟器224个码位。超过的部分会被忽略

```python
sims = Simulator('cfg1') # 参数参考配置内定义的
for i in range(1, 7):
    sims.openwith(i, 'E0')
```

### 11. 集成RUT

  测试平台可调用部分RUT 的接口

  如 打开cop， 打开viv 会话，获取功能模式，获取eventlog

```python
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
            
            viv = VCU_VIV(slot)
            for cpu in ['A', 'B']:
                event = viv.get_eventlog(cpu)  
    mode_dict = form.get_funcMode('3-8')
    form.get_eventlog('2-8,16')       
```

  集成的RUT工具可独立使用

```python
form.update('S19', 0, '3-8')
form.update_dump(module_3, mode)
form.update_cfg(‘2.16’, ‘’) 
def update(fileType: str, act, module_3, dirname = os.path.dirname(__file__), block = True, retry = 5):
    '''
    Descrption: 调用RUT工具
    args:
     fileType:str 镜像的类型如 S19,bin
     act:int|str  {'0': 'image update', '5': 'get version', "4": 'restart'}
     module_3:str|list '3-8,16' or [2,3,4,5,7]
     dirname:str 镜像所在文件的路径, 默认为脚本所在目录，当调用copyfromVM 时会切换至copyfromVM的目标目录
     block:bool  是否等在更新完成
     retry:int   当阻塞时， 失败重传最大次数
    Returns: Type:None(block=True), list(block=False)
     当block 为True 时 无返回值
     当block 为False时 返回一个子进程列表
    '''
```

```batch
rem #更新对象 image 镜像 cfg  配置 dump 宕机日志

rem IMAGE:
rem #镜像的格式，区分大小写 lzw bin S19 auto
rem #操作指令 0文件上传并更新 1文件上传  2恢复更新 3镜像更新 4重启 5获取版本号
rem #镜像所在的CPU_X所在路径
rem #槽道号 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3

rem CFG:
rem # 配置名称
rem # 槽道号 只能包含mvcu 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3

rem DUMP:
rem # 对应CPU 只能为 A B AB
rem # 槽道号 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3

python3 light_update.py image auto 0 . 
::python3 light_update.py image S19 0 . 5
::python3 light_update.py cfg CSZ_ECID_VOOB_6diff
::python3 light_update.py dump AB
pause
```

## 二. 配置

### 1. 运行环境

安装 git (可选)

安装 python3.10 （不支持python2）

![](.\pictures\2021-10-14-16-24-00-image.png)

 安装时 需勾选此处，若未勾选需手动将安装目录添加至环境变量。

 **运行TestPlatForm/venv/ install.bat**, 每次更改TestPlatForm 的名称或路径均需运行install

 解压缩 Vs code 选择file - open folder - TestPlatForm 打开

### 2. udp_loglib.py

```python
class Config:
    slot_ip = {2: '20.2.1.10', 16: '20.1.1.10'} # MVCU 槽道，ip
    local_ip = {2: '20.2.1.100', 16: '20.1.1.100'} # 本地与MVCU 同一网段的ip
    wireshark_path = r"D:\Wireshark" # wireshark 安装路径
    default_ethdesc = 'Realtek PCIe GBE Family Controller' # 本地网卡描述
```

### 3. vmtcp.py

```python
class VMConfig:
    #CONFIG_START
    server_addr = ('20.3.1.50', 18125) # VM 端 server ip
    name_dict = {'svcu': 'VCU_DEV', 'mvcu': 'VCU_DEV_MVCU'} # 代码在VMtools 的 名称
    path_dict = {} # 客户端在启动时会共享path_dict 至服务端
    if RUN_MODE != 'server': 
        path_dict[get_desktop()] = r'E:' # 虚拟机的桌面共享文件夹
        path_dict[getPath_TestPlatform()] = r'E:\TestPlatform' #虚拟机内测试平台的共享文件夹（Master/testplatform）
    #CONFIG_END
```

### 4. caseFunc.py

```python
class Config:
    form = None #type: udp_loglib.TestPlatForm
    slot_ip = udp_loglib.Config.slot_ip #type: dict

    @staticmethod
    def active_mvcu() -> str:
        '2, 2,16 16'
        return '2'

    @staticmethod
    def active_svcu() -> str:
        return '8'

    @staticmethod
    def serial_config() -> dict:
        return {'A': 8, 'B': '8'}

    @staticmethod
    def get_normal_image_path():
        '''获取当前正常镜像的路径'''
        return os.path.abspath('%s/normal_image' % (os.path.dirname(__file__)))

    @staticmethod
    def case_path() -> list:
        '''设定本次执行的用例目录，以及n 级子目录'''
        path1 = os.path.dirname(__file__) + '\\' + 'testcase' 

        deep = 4 # 包含n级子目录
        rt_path = []
        rt_path.append(path1)

        def find_dir(path, deep):
            rt_path = []
            if deep > 0:
                for p in os.listdir(path):
                    p = path + '\\' + p
                    if os.path.isdir(p):
                        rt_path.append(p)
                        rt_path += find_dir(p, deep - 1)
            return rt_path
        rt_path += find_dir(path1, deep)
        return rt_path # 本次自动执行的脚本路径

    @classmethod
    def set_form(cls, form: udp_loglib.TestPlatForm):
        cls.form = form

    @staticmethod
    def get_suffix() -> list:
        '''执行时，用例名称的结尾需满足返回的列表的条件之一，忽略大小写'''
        return ['.PY'], [] # 本次自动执行的脚本 格式（后缀）, 第二个列表为禁止出现的后缀

    @staticmethod
    def get_prefix() -> list: 
        '''执行时会将用例名称按_分割，分割后的字符需满足返回的所有条件，忽略大小写
        return ['SwRTC', 'cfg2'], [] # 本次自动执行的脚本的前缀  ，第二个列表为禁止出现的前缀
```

### 5. light_update.py

```python
slot_ip = {2: '20.2.1.10', 16: '20.1.1.10'}
```

### 6. ip 配置

> AutoPower:  addr=172.16.0.100 mask=255.255.0.0
> 
> 测试平台： 根据配置
> 
> VCU_IP:  根据配置
> 
> 可通过setip.bat 一键设置（默认值，不保证和配置一致）

### 7. 模拟器配置

  模拟器需使用修改后的模拟器(//data_new)

  若无法打开ECID 模拟器， 安装vc_redist.x86.exe（//data_new）

```python
#testform\py_lib/simulator.py
class FSFB2:

    @classmethod
    def dirname(cls, num):
        return 'CC%s' % num

    @classmethod
    def sim_exe(cls, num):
        return 'ECIDSim.exe'

    @classmethod
    def get_name(cls, num):
        return 'SIMU%s' % num

class RSSP1:

    @classmethod
    def dirname(cls, num):
        return 'RSSP1_%s' % num

    @classmethod
    def sim_exe(cls, num):
        return 'RSSP1Sim.exe'  

    @classmethod
    def get_name(cls, num):
        return 'RSSP1-%s' % num

class Config:
    root = r'C:\Users\LiuXintian\Desktop\data_new'
    cfg1 = ('data1-FSFB2-每个节点配置224', FSFB2, FSFB2，FSFB2) # 协议混用怕配置多个类型，最多支持6 个会话
    cfg2 = ('data2-FSFB2-发送码位不同', FSFB2)
    cfg3 = ('data3-RSSP-I-1344 para-6auto-拷机', RSSP1)
    cfg4 = ('data4-RSSP-I-6auto发送码位不同', RSSP1)  
```

### 8. WireShark

**若WireShark 安装在C 盘，需在wireshark 的根目录复制一份dumpcap.exe 并重命名为test_dumpcap.exe**

将WireShark/FSFB2.lua, RSSP1.lua, VCU_TST.lua 存放在wireshark 的根目录，并在根目录打开init.lua, 并在其文件末尾添加

```lua
--if not running_superuser or run_user_scripts_when_superuser then 
    --若 源文件无此if ，则无需加入if
    dofile("FSFB2.lua")
    dofile("RSSP1.lua")
    dofile("VCU_TST.lua")
--end
```

打开WireShark，无报错且 在过滤栏输入 fsfb2 若格式为绿色则配置正确

## 三. 主要函数或方法

```python
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
          vmtool: VMTool 与虚拟机交互
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          srcPort:int 本地端口号
          dstPort:int VCU 端口号
          set_client： 自动设置端口号 默认为[]
        Returns: Type:None
          无返回值
        '''  
    def _set_client(self, mode = [], retry = 5):
        '''description: 指定发送消息的MVCU，当使用两个mvcu 时 发送给奇数槽道的svcu的消息会通过 mvcuS 转发， 发送给偶数svcu 的槽道的消息会通过mvcuP 转发

        Attributes: None

        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          mode:str|list 格式为 '2' '16' '2,16' [2,16] [] 若为空则自动选择
          retry:int 重新连接次数， 若无需重连则设为0
        Returns: Type:None
          无返回值
        '''  
    def get_funcMode(self, slots) -> dict:
        '''description: 发送测试命令的基本类
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          slots: str|list 所需获取的槽道号
        Returns: Type:dict
          dict[slot] = funcmode
        '''  
    def get_active_mvcu(self) -> int:
        '''获取主mvcu 的槽道号''' 
    def get_standby_mvcu(self) -> int:
        '''获取备mvcu 的槽道号''' 
    def get_dump(self, moudle_3, CPU = 'AB', outfile_dir = ''):

    def get_eventlog(self, moudle_3):
        '''获取 vcu eventlog 结果自动保存在‘EventLog’ 文件夹
            moudle_3: str|list 目标槽道
        '''
    @property
    def lru_auto(self):
        '''为True 后 会自动过滤掉不可用的槽道，但因mib 信息存在延迟，可能会不准，耗时根据配置，最大额外耗时为7秒左右'''
        return self._lru_auto

    @lru_auto.setter
    def lru_auto(self, val: bool):
        '''为True 后 会自动过滤掉不可用的槽道，但因mib 信息存在延迟，可能会不准，耗时根据配置，最大额外耗时为7秒左右'''
        self._lru_auto = val 
    @funclog
    def tst_set(self, slot: str, frame: UdpSendFrame, openflag=True, autowait = True) -> None:
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
    @funclog
    def _VCUs_available(self, moudle_3s: list) -> list:
        '''description:获取可用的槽道号        
        Arguments:
          self:instance 调用该函数的实例，无需手动赋值
          moudle_3s：list 原始list 
        Returns: Type:list
          返回原始list 中可用的 槽道号 
        '''  
class Conf(Default):
    @classmethod
    def udpFlagSet(cls, val, mode=3):
        arg = (cls.__flag, cls.__flag_dict['udp'])
        val = cls.modeSet(val, mode)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def serialFlagSet(cls, val, mode=3):
        arg = (cls.__flag, cls.__flag_dict['serial'])
        val = cls.modeSet(val, mode)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def allFlagSet(cls, val, mode=3):
        arg = (cls.__flag, cls.__flag_dict['all'])
        val = cls.modeSet(val, mode)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod 
    def Shutdown(cls):
        arg = (cls.__control, cls.__control_dict['Shutdown'])
        val = cls.modeSet(1, 3)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod 
    def TriggerFaultFifter(cls):
        arg = (cls.__control, cls.__control_dict['TriggerFaultFifter'])
        val = cls.modeSet(1, 3)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def toStandby(cls):
        arg = (cls.__control, cls.__control_dict['toStandby'])
        val = cls.modeSet(1, 3)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def bothCycleSet(cls, val, mode=3):
        arg = (cls.__cycle, cls.__cycle_dict['all'])
        val = cls.modeSet(val, mode)
        return cls._set(arg[0], arg[1], val, auto=True)

    @classmethod
    def _set(cls, arg1, arg2, arg3, auto=False):
        '''description:获得所需的指令str

        Attributes: args, auto

        Arguments:
          args:tuple 内容为整形，以十进制赋值并自动对齐，若为字符串则只有auto 为True 时才会自动对齐
          auto:bool 如果为真且该 func 被允许，将自动打开allFlag，默认为TRUE，无需显式赋值
        Returns: Type:instance of UdpSendFrame
          发送给VCU的指令'''

    @classmethod
    def modeSet(cls, val, mode = 0, default='FFFF') -> str:
        ''' mode = 0 将 val 转换为 长度为8 的字符串，高位补零
            mode = 1:将高四位 置为default,低四位为val
            mode = 2:将低四位 置为default，高四位为val
            mode = 3:高四位、低四位均置为 val
        '''

class Tag(Default):
    @classmethod
    def tag_send(cls, tag: str, tagType='00', cycle=0):
        '''description:生成向VCU打开桩指令

        Attributes: cls, tag, tagType, cycle

        Arguments:
          cls:class 调用该函数的实例，无需手动赋值
          tag:str 格式为 [0-9A-Fa-f]{4}[01]{2}
          tagType:str 暂未用到
          cycle:bool 该桩开始生效的测试周期
        Returns: Type:UdpSendFrame
          生成的指令
        '''  
class VCU:
    ''''''
    lru_ban = [1, 15, 17, 31, 33, 47]  #非法slot ID

    @classmethod
    def moudle_3toNode(cls, moudle_3):
        '''将moudleID减三（即槽道号+（机笼号-1）*16）转为nodeID'''
        moudle_3 = int(moudle_3)
        Public.myAssert(moudle_3 <= 16 * 3 and moudle_3 > 0)
        if moudle_3 == 2:
            nodeId1 = '02'
            nodeId2 = '03'
        elif moudle_3 == 16:
            nodeId1 = '04'
            nodeId2 = '05'
        else:
            nodeId1 = hex((moudle_3 + 3) * 2)[2:]
            nodeId1 = nodeId1.rjust(2, '0')
            nodeId2 = hex((moudle_3 + 3) * 2 + 1)[2:]
            nodeId2 = nodeId2.rjust(2, '0')
        return nodeId1, nodeId2  # 对应槽道的cpua、cpub的nodeid

    @staticmethod
    def inputsplit(slot: str, rack=1):
        '''将输入的moudle — 3 分割成数组
            exmple：
            VCU.inputsplit('2.3,4，5，6-20') -> [2,3,4,5,6,7,...,20]'''
        global lru_ban
        slot = str(slot)
        slot = re.split(r'[,，.]', slot)
        moduleID_3 = []
        for s in slot:
            s = s.split('-')
            s.append(s[0])
            moduleID_3 += range(int(s[0]), int(s[1]) + 1)
        for i in range(len(moduleID_3)):
            if moduleID_3[i] < 17:
                moduleID_3[i] += (int(rack) - 1) * 16
            Public.myAssert(moduleID_3[i] <= 0xFF and moduleID_3[i] > 0,
                            '格式错误')
            # if moduleID_3[i] in lru_ban:
            #     raise Exception('slot input mistake')
            #     moduleID_3[i] = 0
        return [m for m in moduleID_3 if m not in VCU.lru_ban]

    @staticmethod
    def node2lru(arg, sp={-2: '1_2P', -1: '1_16S', 2: '1_2P', 16: '1_16S'}):
        '''将nodeid 转换为lru格式,sp:特殊值
           arg: int|str
        '''
        if isinstance(arg, str):
            arg = int(arg, 16)
        myAssert(arg < 0xFF and arg >= 0)
        cpu = {0: 'CPUA', 1: 'CPUB'}.get(arg % 2)

        arg = arg // 2 - 3  #先转为moudleid - 3 ，sp->key

        x = (arg - 1) // 16 + 1  #机笼号
        y = arg - 16 * (x - 1)  # 槽道号
        return sp.get(arg, '%s_%s' % (str(x), str(y))), cpu

    @staticmethod
    def gettime():
        '''fmt:%Y-%m-%d %H:%M:%S'''
        return datetime.now().strftime('%Y-%m-%d %H:%M:%S')  
class Public(object):
    @staticmethod
    def modify(string: str, indexs: int, value: str) -> str:
        '''修改字符串某几位，Public.modify(str,range(1,2),'123')
        Public.modify(str,'1-2','123')
        不包括2，和range 用法类似'''

    @staticmethod
    def decodeException():
        import tempfile
        tmpfd, tmpname = tempfile.mkstemp()
        with open(tmpname, 'w+') as f:
            traceback.print_exc(file=f)
            f.seek(0)
            lines = f.readlines()
        os.close(tmpfd)
        os.unlink(tmpname)
        #value = sys.exc_info()
        return lines

    @staticmethod
    def int2hex(val: int, width: int) -> str:
        '''将整形转换为一个字节码字符串'''
        #if isinstance(val,str):
        #    val = int(val,16)
        Public.myAssert(val <= int('F' * width, 16))
        temp = hex(val)[2:]  #去掉前缀0x
        temp = re.sub(r'[Ll]', '', temp)  #去掉后缀
        temp = temp.rjust(width, '0')  #高位补零
        return temp.upper()

    @staticmethod
    def isIPreachable(ip, timeout = 1.5):
        '''通过ping命令判断ip是否可以通信，ping中文默认编码方式是GBK，
            out 内中文为乱码，字母可正常显示   
        '''
        p = subprocess.Popen(['ping', '-n', '1', '-w', str( int(timeout * 1000)), ip], stdout=subprocess.PIPE)
        p.wait()
        out = str(p.stdout.read())
        if 'TTL=' in out:
            return True
        else:
            return False

    @classmethod
    @funclog
    def none_func(cls, *args, **kwargs):
        '''global print print = Public.none_func'''
        pass

     @staticmethod
    @funclog
    def mPrint(*args, **kwargs):
    '''测试平台的打印
     args: 
       mode:int [0,1,2,3] 0时两条分割线均打印. 1 仅打印第一条 2： 均不打印 3： 仅打印第三条
    '''

    @staticmethod
    def myAssert(ass: bool, *args, **kwargs):
        '''断言函数'''


    @staticmethod
    def tuple_len(arg: tuple):
        '''计算一个元组里的字符数'''
        Public.myAssert(isinstance(arg, tuple) or isinstance(arg, list))
        length = 0
        for v in arg:
            if isinstance(v, tuple) or isinstance(v, list):
                length += Public.tuple_len(v)
            else:
                length += len(str(v))
        return length

    @staticmethod
    def crc16(data: str, leng):
        '''CCITT-FALSE'''
        seed = 0xFFFF
        poly = 0x1021
```
