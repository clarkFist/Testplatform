#!/usr/bin/env python
#coding=utf-8
from __future__ import print_function
from functools import wraps
from inspect import getframeinfo, stack
default_print = print

@wraps(default_print)
def print(*args, **kwargs):
    '''连续的不换行打印即使是不同文件也被是为同一来源，以第一个不换行为主'''
    if not hasattr(print, 'flag'):
        print.flag = True
    if print.flag is True:
        caller = getframeinfo(stack()[1][0])
        pos = caller.filename.split('\\')[-1]
        msg = "%s %s msg:" % (pos, caller.lineno)
        default_print(msg, end='')
    if 'end' in kwargs.keys() and '\n' not in kwargs['end']:
        print.flag = False
    else:
        print.flag = True
    default_print(*args, **kwargs)
