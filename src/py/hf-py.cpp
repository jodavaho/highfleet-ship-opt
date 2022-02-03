#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>
#include "lib/hf-problem.hpp"
#include "py/hf-py.h"
#include <iostream>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

  static PyObject* version(PyObject* self, PyObject* args){
    return PyUnicode_FromString(hf_opt_version);
  }

  static PyObject * get_module_names(PyObject *self, PyObject *args)
  {
    std::vector<hf::module> vec = hf::get_all_modules();
    PyObject* module_name_list = PyList_New(hf::num_modules());
    assert(hf::num_modules()==vec.size());
    for (size_t i=0;i<hf::num_modules();i++){
      Py_ssize_t idx(i);
      hf::module mod = vec[i];
      PyObject * name = PyUnicode_FromString(mod.name.c_str());
      PyList_SetItem(module_name_list, idx, name);
    }
    return module_name_list;
  }

  static PyObject * solve_fill(PyObject *self, PyObject *args)
  {
    //https://docs.python.org/3/c-api/list.html
    //https://docs.python.org/3/c-api/tuple.html
    //https://docs.python.org/3/c-api/dict.html
    PyObject * mod_counts;
    hf::Bounds b;
    hf::SolveOptions o;
    if (!PyArg_ParseTuple(args, "Offf", &mod_counts, &b.range_min, &b.spd_min, &b.twr_min))
        return NULL;
    Py_INCREF(mod_counts);
    const std::vector<hf::module> all_modules = hf::get_all_modules();
    std::vector<size_t> optimized_counts(hf::num_modules());//populate from minimums

    //extract all modules from the dictionary
    for (size_t idx =0;idx<hf::num_modules();idx++){
      const auto &m=all_modules[idx];
      PyObject* name_str = PyUnicode_FromString(m.name.c_str());
      if (! PyUnicode_Check(name_str)){
        std::cerr<<"PyUnicodeError parsing string from: "<<m.name<<std::endl;
        continue;
      }
      //I long for match statements
      //see if they sent us a required count for this module
      switch(PyDict_Contains(mod_counts,name_str)){
        case (1):{//Contains
                   PyObject*item= PyDict_GetItem(mod_counts,name_str);
                   if (!PyLong_Check(item)){
                     std::cerr<<"PyLongError parsing count for "<<m.name.c_str()<<std::endl;
                     continue;
                   }
                   optimized_counts[idx]=PyLong_AsLong(item);
                   break;
                 }
        case(0):{//!Contains
                  optimized_counts[idx]=0;
                  break;
                }
        case(-1):{//Error
                   break;
                 }
        default:{//wtf
                  break;
                }
      }
    }
    auto retcode = solve(optimized_counts,all_modules,b,o); 
    if (retcode==hf::ERR_INFEASIBLE){
      std::cerr<<"Problem appears infeasible!"<<std::endl;
      return nullptr;
    }
    //repopulate their counters
    for (size_t idx=0;idx<hf::num_modules();idx++){
      const auto &m = all_modules[idx];
      PyObject* name_str=PyUnicode_FromString(m.name.c_str());
      if (! PyUnicode_Check(name_str)){
        //we warned above
        continue;
      }
      PyObject* count = PyLong_FromLong(optimized_counts[idx]);
      if (!count | !PyLong_Check(count)){
        std::cerr<<"Unable to parse long from: "<<optimized_counts[idx]<<std::endl;
        continue;
      }
      switch(PyDict_SetItem(mod_counts,name_str,count)){
        case 0:{break;}
        case -1:{std::cerr<<"Unable to insert "<<count<<" into "<<mod_counts<<std::endl;
                  continue;}
      }
    }
    return mod_counts;
  }

  static PyObject* print_version(PyObject* self, PyObject* args){
    std::cout<<"Version: "<<hf_opt_version<<std::endl;
    Py_RETURN_NONE;
  }

  //TODO change these to METH_FASTCALL with only positional arges (accepting a
  //C array of PyObject*)
  static PyMethodDef hfopt_methods[] = {
    {"solve_fill",  solve_fill, METH_VARARGS, "Solve a 'fill' problem given some modules and design constraints."},
    {"print_version",  print_version, METH_VARARGS, "Print version of underlying library"},
    {"get_module_names",  get_module_names, METH_VARARGS, "Get supported module names"},
    {"version",  version, METH_VARARGS, "Print version"},
    {NULL, NULL, 0, NULL}        /* Sentinel */

  };

  static struct PyModuleDef hfopt_module= {
    PyModuleDef_HEAD_INIT, //Always initialize this member to PyModuleDef_HEAD_INIT.
    "hfopt_lib",  
    PyDoc_STR("A HF Ship Optimization module. See jodavaho.io/highfleet"),
    -1,      
    hfopt_methods
  };

  PyMODINIT_FUNC PyInit_hfopt_lib(void){
    return PyModule_Create(&hfopt_module);
  }

#ifdef __cplusplus
}
#endif
