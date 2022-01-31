from ctypes import cdll

def __init__():
    pass

def __bootstrap__():
   print("loading hfopt")
   global __bootstrap__, __loader__, __file__
   import sys, pkg_resources, imp

   __file__ = pkg_resources.resource_filename(__name__,'hfopt.cpython-38-x86_64-linux-gnu.so')
   imp.load_dynamic(__name__,__file__)

   __loader__ = None; del __bootstrap__, __loader__

print("loading hfopt")
__bootstrap__()
