#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>
#include "lib/hf-problem.hpp"
#include "py/hf-py.h"
#include <iostream>
#include <vector>
#include <unordered_map>

#ifdef __cplusplus
extern "C" {
#endif

  static PyObject* version(PyObject* self, PyObject* args){
    return PyUnicode_FromString(hf_opt_version);
  }

  static PyObject* is_module(PyObject *self, PyObject* args){
    PyObject* name_str;
    if (!PyArg_ParseTuple(args, "O", &name_str)){
        return nullptr;
    }

    if (!PyUnicode_Check(name_str)){
      std::cerr<<"Need a name string! Encountered some other object!"<<std::endl;
    }

    const char* cname = PyUnicode_AsUTF8(name_str);

    const auto mp = hf::by_name(cname);
    if (mp){
      return Py_True;
    } else {
      return Py_False;
    }
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
    PyObject * mod_counts;

    hf::Bounds b;
    hf::SolveOptions o;

    if (!PyArg_ParseTuple(args, "Oddd", &mod_counts, &b.range_min, &b.spd_min, &b.twr_min))
        return NULL;

    std::cout<<b<<std::endl;

    if (!PyDict_Check(mod_counts)){
      std::cerr<<"Did not get module_list as dictionary! Fatal."<<std::endl;
      return nullptr;
    }

    //we're borrowing mod_counts, so let's increment so we can return it
    Py_INCREF(mod_counts);

    //get hf:: data, including module list
    const std::vector<hf::module> all_modules = hf::get_all_modules();
    std::unordered_map<std::string, long> idx_maps;

    //idx lookup table
    for (size_t idx =0;idx<hf::num_modules();idx++){
      //if module[i].name == whatever they requested, how do we find i?
      //we save it here and look it up later
      idx_maps[all_modules[idx].name]=idx;
    }

    //create optimized_counts, to send into solve()
    std::vector<size_t> optimized_counts(hf::num_modules());//populate from minimums

    //mod_counts={{name:count}, {name-again,count-again}, etc}
    //iterate over the keys they passed in, which are supposed to be module names
    PyObject *req_mods = PyDict_Keys(mod_counts);
    if (!PyList_Check(req_mods)){
      std::cerr<<"Error forming list of keys ... fatal!"<<std::endl;
      return nullptr;
    }

    //how many keys?
    size_t req_sz = PyList_Size(req_mods);
    for (size_t i=0;i<req_sz;i++){
      PyObject* module_request = PyList_GetItem(req_mods,i);
      if (!PyUnicode_Check(module_request)){
        std::cerr<<"Expected module name, but was given a non-string object! Trying to continue without it."<<std::endl;
        continue;
      }
      //get module by name
      const char* cname = PyUnicode_AsUTF8(module_request);
      auto requested = hf::by_name(cname);
      if (!requested){
        std::cerr<<"Unrecognized module name: "<<cname<<". Continuing ... "<<std::endl;
        continue;
      }
      //we had an idx for this module, right?
      assert(idx_maps.find(requested->name) != idx_maps.end());
      size_t idx = idx_maps[requested->name];
      //fetch count as requested_count = modules[name]
      PyObject* requested_count= PyDict_GetItem(mod_counts,module_request);
      if (!PyLong_Check(requested_count)){
        std::cerr<<"PyLongError parsing count for "<<cname<<". Trying to continue without it. "<<std::endl;
        continue;
      }
      optimized_counts[idx]+=PyLong_AsLong(requested_count);//allow multiple name entries by adding all references to a given module together.
    }

    auto retcode = solve(optimized_counts,all_modules,b,o); 
    if (retcode==hf::ERR_INFEASIBLE){
      std::cerr<<"Problem appears infeasible!"<<std::endl;
      return nullptr;
    }
    //repopulate their counters
    PyDict_Clear(mod_counts);
    for (size_t idx=0;idx<hf::num_modules();idx++){
      const auto &m = all_modules[idx];
      PyObject* name_str=PyUnicode_FromString(m.name.c_str());
      if (! PyUnicode_Check(name_str)){
        std::cerr<<"Error constructing unicode string from '"<<m.name<<"'. Trying to continue ... "<<std::endl;
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
    {"is_module",  is_module, METH_VARARGS, "Check if this is a module name"},
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
