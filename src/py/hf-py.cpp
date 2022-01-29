#define PY_SSIZE_T_CLEAN
#include <python3.8/Python.h>
#include "lib/hf-problem.hpp"

//#ifdef __cplusplus
//extern "C" {
//#endif


  static PyObject * solve_fill(PyObject *self, PyObject *args)
  {
    int num_req_mods=0;
    if (!PyArg_ParseTuple(args, "i", &num_req_mods))
        return NULL;
    //PyErr_SetString();
    return nullptr;
  }

  static PyMethodDef hfopt_methods[] = {
    {"hfopt",  solve_fill, METH_VARARGS,
      "Solve a 'fill' problem given some modules and design constraints."},
    {NULL, NULL, 0, NULL}        /* Sentinel */

  };

  static struct PyModuleDef hfopt_module= {
    PyModuleDef_HEAD_INIT,
    "hfopt",   /* name of module */
    nullptr, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    hfopt_methods
  };

  PyMODINIT_FUNC PyInit_hfopt(void){
    PyObject *m;
    m = PyModule_Create(&hfopt_module);
    //pass
    return m;
  }

  Bounds init_bounds(){
    Bounds b;
    return b;
  }

  SolveOptions init_options()
  {
    SolveOptions opts;
    return opts;
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


//#ifdef __cplusplus
//}
//#endif
