#ifndef read_problem_hpp
#define read_problem_hpp

#include <iostream>
#include <istream>
#include <vector>
#include "hf-problem.hpp"


void read_from(
    std::istream &s,
    Bounds &out_bounds,
    std::vector<module> &out_required,
    SolveOptions &out_opts
    );

void read_from_stdin(
    Bounds &out_bounds,
    std::vector<module> &out_required,
    SolveOptions &out_opts
    );

#endif

