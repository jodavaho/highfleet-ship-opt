from distutils.core import setup, Extension
import sys

#https://stackoverflow.com/a/49139257
static_libraries = ['scip','hf']
static_lib_dir = '.deps/x86_64-linux-gnu/lib'
libraries = ['z','gmp']
include_dirs=['src','src/lib']
library_dirs = ['/lib','/usr/lib','/system/lib', '/system/lib64']

if sys.platform == 'win32':
    libraries.extend(static_libraries)
    library_dirs.append(static_lib_dir)
    extra_objects = []
else: # POSIX
    extra_objects = ['{}/lib{}.a'.format(static_lib_dir, l) for l in static_libraries]

module1 = Extension('hfopt', sources = [
    'src/py/hf-py.cpp',
    #'src/lib/modules.cpp', # included via library, above
    #'src/lib/hf-problem.cpp'
    ],
    libraries=libraries,
    library_dirs=library_dirs,
    include_dirs=include_dirs,
    extra_objects=extra_objects,
    extra_compile_args=['-fPIC','-std=c++2a','-I.deps/x86_64-linux-gnu/include','-DNO_CONFIG_HEADER'],
    )

setup (name = 'Highfleet Ship Opt',
       version = '1.0',
       description = '',
       ext_modules = [module1])
