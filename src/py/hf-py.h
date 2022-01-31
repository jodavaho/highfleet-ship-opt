#ifndef hf_py_h
#define hf_py_h

#include "../lib/hf-problem.hpp"
#include <python3.8/Python.h>

#ifdef __cplusplus
extern "C" {
#endif

  PyObject* print_version(PyObject* self, PyObject* args);

  PyObject* solve_simple(int num_req_mods, char** req_mod_names, int* req_mod_counts, Bounds bounds, SolveOptions opts);

#ifdef __cplusplus
}
#endif

#endif
