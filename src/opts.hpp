#ifndef opts_hpp
#define opts_hpp

#include "lib/hf-problem.hpp" //Bounds
#include "lib/modules.hpp"    //Module names
#include <string>
#include <vector>

using hf::Bounds;
using hf::SolveOptions;
using hf::module;

std::string get_help();

bool parse_opts(
    int argc, char** argv, 
    struct Bounds& out_bounds, 
    struct SolveOptions&  out_opts,
    std::vector<struct module> & out_modules
    );

std::string get_credits();

#endif
