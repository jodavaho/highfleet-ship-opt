from distutils.core import setup, Extension
import sys

#https://stackoverflow.com/a/49139257
#static_libraries = [ 'hf', ]
#static_lib_dir = '.deps/x86_64-linux-gnu/lib'
libraries = ['z','gmp','scip']
include_dirs=['src','src/lib']
library_dirs = ['.deps/x86_64-linux-gnu/lib','/lib','/usr/lib','/system/lib', '/system/lib64']

#We don't need static libs at the moment, may go back
#if sys.platform == 'win32':
#    libraries.extend(static_libraries)
#    library_dirs.append(static_lib_dir)
#    extra_objects = []
#else: # POSIX
#    extra_objects = ['{}/lib{}.a'.format(static_lib_dir, l) for l in static_libraries]

module1 = Extension('hfopt_lib', sources = [
    'src/py/hf-py.cpp',
    'src/lib/modules.cpp', 
    'src/lib/hf-problem.cpp'
    ],
    libraries=libraries,
    library_dirs=library_dirs,
    include_dirs=include_dirs,
    #extra_objects=extra_objects, for static libs
    extra_compile_args=['-Werror','-fPIC','-std=c++2a','-I.deps/x86_64-linux-gnu/include','-DNO_CONFIG_HEADER'],
    )

setup(name = 'hfopt_lib',
       version = '0.1.1',
       description = "HF Ship Opt.",
       ext_modules = [module1])
