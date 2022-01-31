def __bootstrap__():
   global __bootstrap__, __loader__, __file__
   import sys, pkg_resources, imp

   __file__ = pkg_resources.resource_filename(__name__,'libscip.so')
   imp.load_dynamic(__name__,__file__)

   __file__ = pkg_resources.resource_filename(__name__,'libhf.so')
   imp.load_dynamic(__name__,__file__)

   __file__ = pkg_resources.resource_filename(__name__,'hfopt.cpython-38-x86_64-linux-gnu.so')
   imp.load_dynamic(__name__,__file__)

   __loader__ = None; del __bootstrap__, __loader__
__bootstrap__()
