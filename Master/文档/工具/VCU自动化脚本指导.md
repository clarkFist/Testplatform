### 自动化脚本基础

首先完成caseFunc.py 内的配置， 且准备好自动执行的脚本，**运行case_test/main.py**

测试脚本以**case_test/testcase/default.py 为模板**， 自动执行仅会执行main 函数

测试脚本需按照Cfg2_SyRTC_0001.py, Cfg_SwITC_SVCU_0002.ad 格式命名

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
import shutil
    dir_path = os.path.dirname(__file__) + '/Log/%s/%s' % (os.path.splitext(os.path.basename(__file__))[0], '')
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)
    LogPath_set('default')
'''''''''''' # 以上为固定头
requirement = caseFunc.StubRequirement(slots=None, svcu_count=None) #slots='2,3-8'，该变量必须赋值, 当前用例的环境需求 
#requirement = caseFunc.StubRequirement(slots='2.16, 5', svcu_count=5) 
# 该行自动执行前会检查caseFunc.Config是否配置了2.16.5，caseFunc.Config 配置了 svcu 数量至少为5
#step1():
#    pass
#step2():
#    pass
#step3():
#    pass
#step4():
#    pass
#step5():
#    pass
def main():# 测试脚本入口
    gloabl mvcus
    gloabl svcus
    #svcus = get_svcu()
    #mvcus = caseFunc.Config.active_mvcu()
    #for i in range(1, 6):
    #    eval('step%s' % i)() 
```

脚本若不限制svcu 具体槽道，建议以get_svcu() 函数获取 ,可通过该列表获取指定数量的svcu

可通过form.get_active_mvcu() 获取主mvcu form.get_standby_mvcu() 获取备系mvcu

### 用例的log需要串口

  一些用例需要初始化或宕机时的log， 此时， 可配置脚本的requirement 属性

使用对应串口时确保串口存在且未被其他程序占用， 串口配置参考CaseFunc.Config

```python
# 需要一块svcu 的 A 通道打印
requirement = caseFunc.StubRequirement(serial={'SvcuA': 1})
 svcu = caseFunc.get_slots_withSerial('svcu')['A'][0] 
# 需要两块svcu 的 AB 通道打印
requirement = caseFunc.StubRequirement(serial={'SVCUab': 2})
svcus = caseFunc.get_slots_withSerial('svcu')['AB'][:2] 
# 需要槽道 8 的AB 通道
requirement = caseFunc.StubRequirement(serial={'A': 8, 'B': 8})
requirement = caseFunc.StubRequirement(serial={'AB': 8})
svcus = 8 # '8' [8] 
# 需要一块mvcu 的B 通道
requirement = caseFunc.StubRequirement(serial={'mvcuB': 1})
 mvcu = caseFunc.get_slots_withSerial('mvcu')['B'][0] 
# 其他用法
 requirement = caseFunc.StubRequirement(serial={'mvcuB': 1, 'A': '3-6', 'B':'6-8'})
mvcus, svcus =  caseFunc.get_slots_withSerial()
mvcuB = mvcus['B'][0]
svcuA = svcus['A']
svcuB = svcus['B']
    # 此行代表需要2个 svcuA, 一个 SVCUB，配置AB 的板卡最少一块， 最少4 块svcu 5 根串口线
 requirement = caseFunc.StubRequirement(serial={'svcuA': 2, 'svcuB': 1, 'svcuAB':1})
svcus = caseFunc.get_slots_withSerial('svcu')
svcuAB = svcus['AB'][0]
svcuAs= svcus['_A'][:2] # '_A' 不包括AB 均有串口线的 ‘A’ 既包括但独配置A 也包括 单独配置B
svcuB = svcus['_B'][0]
```

### 用例会导致VCU 宕机

```python
form.before_dump(slots) # 仅枚举桩宕机时调用
#通过测试平台发送的 ShutDown， TriggerFaultFifter, AllowFaultFifter 会自动调用该函数
#根据情况可选择串口打印或待重启完成后掉用
caseFunc.power_restart()
form.get_eventlog(slots)
# 该函数会将对应槽道号的AB 双通道的event log 保存在设定的目录下
```

### 检查用例Log

```python
    while True:
        try:
            info, recvVal = caseFunc.log_queue_get(7, 'A').get(block = True, timeout=2) # 该队列会保存最新500条消息
        except caseFunc.myQueue.Empty:
            continue
        else:
            print(info, recvVal) 
```

## 常用接口

```python
sims = Simulator('cfg1') # 'cfg1' 取决于 py_lib/simulator.py 的配置
for i in range(1, 7):
#若模拟器已打开会重新打开模拟器
   sims.openwith(i, 'E4') # 发送数据为11100011 若不发送数据需填‘for0’ ， ‘’ 空字符串表示采用上一次的值
 for i in range(1, 7):
   sims.close(i)
LogPath_set('02') # 将log 目录设置为以当前文件名为目录名的子目录‘02’ 及 ‘./filename/02/’
form.tst_set(svcu, Tag.tag_send('10320111'))
form.tst_set(2, Conf.toStandby())

#对svcu ‘5-8’ 重新编译并更新， 打开['VCU_SVCU_SwITC_1035_1', 'VCU_SVCU_SwITC_1035_2']这两个宏定义桩， 并阻塞至
#目标板卡进入NORMAL, 默认超时时间为镜像上传完成后200秒
caseFunc.vm_build_update(‘5-8’, ['VCU_SVCU_SwITC_1035_1', 'VCU_SVCU_SwITC_1035_2'], wait='NORMAL')

# 该函数不会抛出异常， 返回值为布尔型， 若强制要求则需加 assert
# 目标板卡需在200s 内全部进入限制态
assert(check_allboard(slots, timeout=200, dstmode=‘LIMATION’) )

#获取mib 
form.mib.get_value() # 参考VCU_TEST.md/html
#以下下方法使用VIV 接口， 在VCU无tst_task 时也可使用
form.update('auto', 1, dst) # 使目标板卡进入REPROG 模式
form.get_eventlog(slots) # 获取 eventlog
mode_dict = form.get_funcMode(slots) #获取目标板卡的功能模式
mvcu_a = form.get_active_mvcu()
mvcu_s = form.get_standby_mvcu()
```

```


