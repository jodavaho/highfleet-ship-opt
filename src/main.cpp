#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "modules.hpp"
#include "opts.hpp"
#include "hf-problem.hpp"

int execopt(int argc, char** argv){
  std::vector<size_t> counts;
  std::vector<module> available_mods;
  for (const auto m: get_all_modules()){
    available_mods.push_back(m);
  }
  Bounds b;
  b.twr[1]=5;
  b.twr[0]=1;
  b.range[0]=1000;
  b.speed[0]=400;
  std::vector<module> req;

  for (int argi=1;argi<argc;argi++){
    auto req_mod = by_name(argv[argi]);
    if (req_mod){
      req.push_back(*req_mod);
    }
  }
  SolveOptions opts;
  opts.include_hull=true;
  return solve(counts, available_mods, b, req, opts );
}

int main(int argc, char** argv){
  return execopt(argc,argv)!=SCIP_OKAY ? 1:0;
}
