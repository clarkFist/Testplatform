import threading
class Singletontype(type):
    _ins_lock = threading.Lock()

    def __call__(cls, *args, **kwargs):
        if not hasattr(cls, "_instance"):
            with Singletontype._ins_lock:
                if not hasattr(cls, "_instance"):
                    cls._instance = super(Singletontype, cls).__call__(*args, **kwargs)
        return cls._instance