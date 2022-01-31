#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>
#include "lib/hf-problem.hpp"
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif


  static PyObject * solve_fill(PyObject *self, PyObject *args)
  {
    int num_req_mods=0;
    if (!PyArg_ParseTuple(args, "i", &num_req_mods))
        return NULL;
    //PyErr_SetString();
    return PyLong_FromLong(0);
  }

  static PyObject* print_version(PyObject* self, PyObject* args){
    std::cout<<"Version"<<std::endl;
    return PyLong_FromLong(0);
  }

  static PyMethodDef hfopt_methods[] = {
    {"solve_fill",  solve_fill, METH_VARARGS, "Solve a 'fill' problem given some modules and design constraints."},
    {"version",  print_version, METH_VARARGS, "Print version"},
    {NULL, NULL, 0, NULL}        /* Sentinel */

  };

  static struct PyModuleDef hfopt_module= {
    PyModuleDef_HEAD_INIT,
    "hfopt",  
    "A HF Ship Optimization module. See jodavaho.io/highfleet", 
    -1,      
    hfopt_methods
  };

  PyMODINIT_FUNC PyInit_hfopt(void){
    return PyModule_Create(&hfopt_module);
  }

  PyObject* solve_fill_simple(int num_req_mods, char** req_mod_names, int* req_mod_counts, Bounds bounds, SolveOptions opts)
  {
    std::vector<module> reqs;
    std::vector<size_t> counts;
    std::vector<module> all;

    for (auto m: get_all_modules()){
      all.push_back(m);
    }

    for (int i=0;i<num_req_mods;i++){
      char* name = req_mod_names[i];
      //int count = req_mod_counts[i]; 
      auto modp = by_name(name);
      if (!modp){
        return nullptr;
      }
      reqs.push_back(*modp);
    }
    SOLVECODE c = solve(counts, all, bounds, reqs, opts);
    if (c == OK){
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
