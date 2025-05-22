from functools import wraps
from inspect import getframeinfo, stack
import os
import re
from datetime import datetime
from types import MethodType
import sys
from singletontype import Singletontype

def gettime():
    '''fmt:%Y-%m-%d %H:%M:%S'''
    return datetime.now().strftime('%Y-%m-%d %H:%M:%S')
def getday():
    return datetime.now().strftime('%Y-%m-%d')
class FuncLog(metaclass=Singletontype):
    root_file = os.path.dirname(os.path.dirname(__file__)) + '/Log_Log/funclog/' 

    def __init__(self, path = '', name= sys.modules['__main__'].__file__.split('\\')[-1] ):
        self.filepath = self.root_file + path + '/' + name + '/' + '%s.log' % name
        if os.path.isfile(self.filepath):
            os.remove(self.filepath)
        self.__sub_path = None

    def __call__(self, func):
        @wraps(func)
        def run(*args, **kwargs):
            _stack = stack()
            try:
                caller = getframeinfo(_stack[1][0])
            except IndexError:
                caller = getframeinfo(_stack[0][0])
            filename = caller.filename.split('\\')[-1]
            log = ' '.join( (gettime(), filename, str(caller.lineno), caller.function, func.__name__,
                            re.sub(r'#.*$', '', ' '.join(caller.code_context).replace('\n', '\\n') ) ) )
            if len(args) > 0 and 'object at 0x' in repr(args[0]):
                args_log = args[1:]
            else:
                args_log = args[:]

            args_log = re.sub(r'at 0x[^>]*', '', str(args_log) )
            kwargs_log = str(kwargs)
            kwargs_log = re.sub(r'at 0x[^>]*', '', kwargs_log)
            with open(self.filepath, 'a') as f:
                f.write(' '.join( ( log, args_log, kwargs_log ) ) + '\r\n' )
            if self.sub_filepath is not None:
                with open(self.sub_filepath, 'a') as f:
                    f.write(' '.join( ( log, args_log, kwargs_log ) ) + '\r\n' )
            return func(*args, **kwargs)
        
        return run

    def wirte(self, *args, **kwargs):
        caller = getframeinfo(stack()[1][0])
        filename = caller.filename.split('\\')[-1]
        log = ' '.join( (gettime(), filename, str(caller.lineno), caller.function, 'funclog.write',
                        re.sub(r'#.*$', '', ' '.join(caller.code_context).replace('\n', '\\n') ) ) )
        if len(args) > 0 and 'object at 0x' in repr(args[0]):
            args_log = args[1:]
        else:
            args_log = args[:]

        args_log = re.sub(r'at 0x[^>]*', '', str(args_log) )
        kwargs_log = str(kwargs)
        kwargs_log = re.sub(r'at 0x[^>]*', '', kwargs_log)
        with open(self.filepath, 'a') as f:
            f.write(' '.join( ( log, args_log, kwargs_log ) ) + '\r\n' )
        if self.sub_filepath is not None:
            with open(self.sub_filepath, 'a') as f:
                f.write(' '.join( ( log, args_log, kwargs_log ) ) + '\r\n' )

    @property
    def filepath(self):
        return self.__path
    
    @filepath.setter
    def filepath(self, val):
        dirname = os.path.dirname(val)
        try:
            if not os.path.exists(dirname):
                os.makedirs(dirname)
        except Exception as e:
            raise e
        self.__path = val
    
    @property
    def sub_filepath(self):
        return self.__sub_path
    
    @sub_filepath.setter
    def sub_filepath(self, val):
        dirname = os.path.dirname(val)
        try:
            if not os.path.exists(dirname):
                os.makedirs(dirname)
        except Exception as e:
            raise e
        self.__sub_path = val
if __name__ == "__main__":
    funclog = FuncLog()
    b = FuncLog()
    c = FuncLog()
    d = FuncLog()
    e = FuncLog()