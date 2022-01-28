#ifndef opts_hpp
#define opts_hpp

#include "lib/hf-problem.hpp" //Bounds
#include "lib/modules.hpp"    //Module names
#include <string>
#include <vector>

std::string get_help();

void parse_opts(
    int argc, char** argv, 
    struct Bounds& out_bounds, 
    std::vector<struct module> & out_modules
    );

std::string get_credits();

#endif
