#include "hf-py.h"
#include <python3.8/Python.h>

extern "C" {
  Bounds init_bounds(){
    Bounds b;
    return b;
  }

  SolveOptions init_options()
  {
    SolveOptions opts;
    return opts;
  }

  PyObject* solve_simple(int num_req_mods, char** req_mod_names, int* req_mod_counts, Bounds bounds, SolveOptions opts)
  {
    std::vector<module> reqs;
    std::vector<size_t> counts;
    std::vector<module> all;

    for (auto m: get_all_modules()){
      all.push_back(m);
    }


    for (int i=0;i<num_req_mods;i++){
      char* name = req_mod_names[i];
      int count = req_mod_counts[i]; 
      auto modp = by_name(name);
      if (!modp){
        return nullptr;
      }
      reqs.push_back(*modp);
    }
    SOLVECODE c = solve(counts, all, bounds, reqs, opts);
    if (c == OK){
      PyObject* py_list = PyList_New(counts.size());
      for (int i=0;i<counts.size();i ++){
        PyObject* py_tup = Py_BuildValue("si",counts[i], all[i].name);
        PyList_SetItem(py_list,i,py_tup);
      }
    }
    return nullptr;
  }
}
