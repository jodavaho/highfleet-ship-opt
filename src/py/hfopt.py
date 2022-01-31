import hfopt_lib
import ctypes

libc = ctypes.CDLL(ctypes.util.find_library('c'))
hfopt.version.argtypes=()
hfopt.version.restype=ctypes.c_char_p
libc.free.argtypes = (ctypes.c_void_p,)

def version():
    _v = hfopt_lib.version()
    result = _v.value
    libc.free(_v)
    return result

class hfopt:
    def __init__(self):
        pass

