#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>
#include "lib/hf-problem.hpp"
#include "py/hf-py.h"
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

  static PyObject* version(PyObject* self, PyObject* args){
    return PyUnicode_FromString(hf_opt_version);
  }

  static PyObject * solve_fill(PyObject *self, PyObject *args)
  {
    int num_req_mods=0;
    if (!PyArg_ParseTuple(args, "i", &num_req_mods))
        return NULL;
    Py_RETURN_NONE;
  }

  static PyObject* print_version(PyObject* self, PyObject* args){
    std::cout<<"Version: "<<hf_opt_version<<std::endl;
    Py_RETURN_NONE;
  }

  static PyMethodDef hfopt_methods[] = {
    {"solve_fill",  solve_fill, METH_VARARGS, "Solve a 'fill' problem given some modules and design constraints."},
    {"print_version",  print_version, METH_VARARGS, "Print version"},
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
      PyObject* py_list = PyList_New(counts.size());
      for (size_t i=0;i<counts.size();i ++){
        PyObject* py_tup = Py_BuildValue("si",all[i].name,counts[i]);
        PyList_SetItem(py_list,i,py_tup);
      }
      return py_list;
    }
    return nullptr;
  }


#ifdef __cplusplus
}
#endif
