# coding=utf-8

import sys
if sys.version_info > (3, 0):
    from collections.abc import Iterable
else:
    from collections import Iterable
import gevent
from gevent import monkey, greenlet
monkey.patch_all()
from functools import wraps
from inspect import getframeinfo, stack
from types import FunctionType, MethodType

import threading
import copy


class Trys(object):
    '''默认采用gevent，当结果不是期望的值时return Trys.todoExpetion('')，默认返回正确值的第一个;
    Examples
    --------
        from trys import Trys
        @Trys([1,2,3])  #'abc'->[a,b,c]
        def example(a,todo): #todo 必须有，不必赋值
            if a < todo:
                return todo
            else:
                return Trys.todoExpetion('')#若全都返回异常，则会抛出该异常
        print example(1)
        >>> 2
        print example(1,todo =3) #显示赋值会覆盖装饰器的初始迭代器的值 'abc' -> ['abc']
        >>> 3
        @Try([1,2,3],'all')#默认值为’first‘
        .....
        print example(1)
        >>> [2,3]
        @Try([1,2,3],lambda _:max(_))#参数为所有正确返回值构成的一个列表
        .....
        print example(1)
        >>> 3
    
    '''
    def __init__(self, todo = [], target = 'todo', rt_type='first'):
        if isinstance(rt_type, FunctionType) or isinstance(rt_type, MethodType):
            self.rt_type = rt_type
        else:
            self.rt_type = eval('self.get_' + rt_type)
        self.todo = todo
        self.mode = self.todoGreenlet
        self.target = target
        
    def __call__(self, func):
        
        @wraps(func)
        def start(*argv, **kwargs):
            if self.target in kwargs:
                li = kwargs[self.target]
                if not isinstance(li, Iterable) or isinstance(li, str):
                    li = [li, ]
            else:
                li = list(self.todo)
            ts = []  # threads
            rt = []  # return value
            for s in li:
                kwargs[self.target] = s
                ts.append(self.mode(func, *copy.deepcopy(argv), **copy.deepcopy(kwargs)))
            for t in ts:
                t.start()
            if self.rt_type.__name__ == 'get_first':
                while len(ts) > 0:
                    for t in ts[:]:
                        if t.dead:
                            tem = t.get()
                            if isinstance(tem, Trys.todoException):
                                ts.remove(t)
                            else:
                                rt.append(tem)
                                ts = []
                                break
                        gevent.sleep(0.02)
            else:
                self.mode.joinall(ts)
                for t in ts:
                    tem = t.get()
                    if not isinstance(tem, Trys.todoException):
                        rt.append(tem)
                    #else:
                    #    e = tem
                        
            if len(rt) == 0:
                rt.append(None)
            return self.rt_type(rt)
        return start

    def get_first(self, arg):
        return arg[0]

    def get_all(self, arg):
        return arg

    class todoException(BaseException):
        def __init__(self, s='Trys.todoException'):
            self.src = str(s)
            caller = getframeinfo(stack()[1][0])
            pos = caller.filename.split('\\')[-1]
            self.caller = "%s %s: " % (pos, caller.lineno)

        def __str__(self):
            return self.caller + self.src

        def __repr__(self):
            return self.caller + self.src

    class todoGreenlet(greenlet.Greenlet):
        
        # def __init__(self, target, args=(), kwargs={}):
        #     self.func = target
        #     self.args = args
        #     self.kwargs = kwargs
        #     super(Trys.todoGreenlet, self).__init__()
        @classmethod
        def joinall(cls, greenlets, timeout=None, raise_error=False, count=None):

            greenlet.joinall(greenlets,
                             timeout=timeout,
                             raise_error=False,
                             count=count)

    class todoThread(threading.Thread):
        @classmethod
        def joinall(cls, ts):
            for t in ts:
                t.join()

        def __init__(self, target, args=(), kwargs={}):
            super(Trys.todoThread, self).__init__()
            self.func = target
            self.args = args
            self.kwargs = kwargs

        def run(self):
            self.rt = self.func(*self.args, **self.kwargs)

        def get(self):
            return self.rt
