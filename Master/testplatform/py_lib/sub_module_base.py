from __future__ import print_function
class SubMoudleBase(object):
    def __init__(self):
        pass
    
    def __new__(cls, *args, **kwargs):
        print_func = kwargs.get('print_func', print)
        file_root_get = kwargs.get('file_root_get', None)
        # if 'print_func' in kwargs.keys():
        #     kwargs.pop('print_func')
        # if 'file_root_get' in kwargs.keys():
        #     kwargs.pop('file_root_get')
        
        instance = object.__new__(cls)
        instance.__print_func = print_func
        instance.__file_root_get = file_root_get
        instance.print = print_func
        return instance

    def print(self, *args, **kwargs):
        self.__print_func(*args, **kwargs)
    
    def get_path(self, outfile):
        if ':' not in outfile[:2] and self.__file_root_get is not None:
            root = self.__file_root_get()
            if not root.endswith('/') and not root.endswith('\\'):
                root += '\\'
        else:
            root = ''
        return root + outfile


if __name__ == "__main__":
    
    class sub(SubMoudleBase):

        def __init__(self, a, **kwargs):
            print(a)
    a = sub(1)
    print(a)
