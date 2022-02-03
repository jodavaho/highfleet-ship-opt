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
    const std::vector<hf::module> all_modules = hf::get_all_modules();
    std::vector<size_t> optimized_counts(hf::num_modules());//populate from minimums

    //extract all modules from the dictionary
    for (const auto &m: all_modules){
      PyObject* name_str = PyUnicode_FromString(m.name.c_str());
      if (! PyUnicode_Check(name_str)){
        //oops
      }
      //I long for match statements
      switch(PyDict_Contains(mod_counts,name_str)){
        case (1):{//Contains
                   PyObject*item= PyDict_GetItem(mod_counts,name_str);
                   if (!PyLong_Check(item)){
                     //oops
                   }
                   break;
                 }
        case(0):{//!Contains
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
    (void)retcode;
    Py_RETURN_NONE;
  }

  static PyObject* print_version(PyObject* self, PyObject* args){
    std::cout<<"Version: "<<hf_opt_version<<std::endl;
    Py_RETURN_NONE;
  }

  static PyMethodDef hfopt_methods[] = {
    {"solve_fill",  solve_fill, METH_VARARGS, "Solve a 'fill' problem given some modules and design constraints."},
    {"print_version",  print_version, METH_VARARGS, "Print version"},
    {"get_module_names",  get_module_names, METH_VARARGS, "Print version"},
    {"version",  version, METH_VARARGS, "Print version"},
    {NULL, NULL, 0, NULL}        /* Sentinel */

  };

  static struct PyModuleDef hfopt_module= {
    PyModuleDef_HEAD_INIT,
    "hfopt_lib",  
    "A HF Ship Optimization module. See jodavaho.io/highfleet", 
    -1,      
    hfopt_methods
  };

  PyMODINIT_FUNC PyInit_hfopt_lib(void){
    return PyModule_Create(&hfopt_module);
  }

  PyObject* solve_fill_ctypes(int num_req_mods, char** req_mod_names, int* req_mod_counts, hf::Bounds bounds, hf::SolveOptions opts)
  {
    assert(num_req_mods == hf::num_modules());
    std::vector<hf::module> reqs;
    std::vector<size_t> counts;
    std::vector<hf::module> all;

    for (auto m: hf::get_all_modules()){
      all.push_back(m);
    }

    for (int i=0;i<num_req_mods;i++){
      char* name = req_mod_names[i];
      //int count = req_mod_counts[i]; 
      auto modp = hf::by_name(name);
      if (!modp){
        return nullptr;
      }
      reqs.push_back(*modp);
    }
    hf::SOLVECODE c = solve(counts, all, bounds, reqs, opts);
    if (c == hf::OK){
      PyObject* py_dict = PyDict_New();
      assert(py_dict);
      for (size_t i=0;i<counts.size();i ++){
        PyDict_SetItem(py_dict,
            PyUnicode_FromString(req_mod_names[i]),
            Py_BuildValue("l",counts[i])
            );
      }
      return py_dict;
    }
    return nullptr;
  }


#ifdef __cplusplus
}
#endif
