from distutils.core import setup, Extension
import sys

#https://stackoverflow.com/a/49139257
static_libraries = [
        #'scip','scipbase',
        'hf',
        #'scip',
        #'lpispx2-8.0.0.linux.x86_64.gnu.opt',
        #'lpispx2',
        #'lpispx2.linux.x86_64.gnu.opt',
        #'objscip-8.0.0.linux.x86_64.gnu.opt',
        #'objscip',
        #'objscip.linux.x86_64.gnu.opt',
        #'scip-8.0.0.linux.x86_64.gnu.opt.spx2',
        #'scip',
        #'scipbase-8.0.0.linux.x86_64.gnu.opt',
        #'scipbase',
        #'scipbase.linux.x86_64.gnu.opt',
        #'scip.linux.x86_64.gnu.opt.spx2',
        #'scipopt',
        #'scipopt-.linux.x86_64.gnu.opt',
        #'soplex-6.0.0.linux.x86_64.gnu.opt',
        #'soplex',
        #'soplex.linux.x86_64.gnu.opt',
        #'tpinone-8.0.0.linux.x86_64.gnu.opt',
        #'tpinone',
        #'tpinone.linux.x86_64.gnu.opt',
        #'zimpl.linux.x86_64.gnu.opt',
        ]

static_lib_dir = '.deps/x86_64-linux-gnu/lib'
libraries = ['z','gmp','scip']
include_dirs=['src','src/lib']
library_dirs = ['.deps/x86_64-linux-gnu/lib','/lib','/usr/lib','/system/lib', '/system/lib64']

if sys.platform == 'win32':
    libraries.extend(static_libraries)
    library_dirs.append(static_lib_dir)
    extra_objects = []
else: # POSIX
    extra_objects = ['{}/lib{}.a'.format(static_lib_dir, l) for l in static_libraries]

module1 = Extension('hfopt_lib', sources = [
    #'src/py/hf-lib.pyx',
    'src/py/hf-py.cpp',
    'src/lib/modules.cpp', # included via library, above
    'src/lib/hf-problem.cpp'
    ],
    libraries=libraries,
    library_dirs=library_dirs,
    include_dirs=include_dirs,
    extra_objects=extra_objects,
    extra_compile_args=['-Werror','-fPIC','-std=c++2a','-I.deps/x86_64-linux-gnu/include','-DNO_CONFIG_HEADER'],
    )

setup(name = 'hfopt_lib',
       version = '0.1.1',
       description = "HF Ship Opt.",
       ext_modules = [module1])
