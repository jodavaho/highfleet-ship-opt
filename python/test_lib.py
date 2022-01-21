from ctypes import *

class SolveOptions(Structure):
    _fields_ = [
            ('include_hull', c_bool),
            ]

class Bounds(Structure):
    _fields_ = [ 
            ('cost', c_double *2),
            ('weight', c_double *2),
            ('speed', c_double *2),
            ('range', c_double *2),
            ('twr', c_double *2),
            ('combat_time', c_double *2),
            ('min_pwr_ratio', c_double *2),
            ('min_crew_ratio', c_double *2),
            ]
    def __repr__(self):
        return ""

libhf = ctypes.CDLL('./libhf.so')
solve_problem = wrap_function(libhf,'solve',
