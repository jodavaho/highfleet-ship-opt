#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "modules.hpp"
#include "opts.hpp"
#include "hf_problem.hpp"

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
  b.speed[0]=100;
  std::vector<module> req;
  //req.push_back( *by_name("RD_51") );
  //req.push_back( *by_name("d_80") );
  req.push_back( *by_name("FuelLarge") );
  Options opts;
  opts.include_hull=true;
  return solve(counts, available_mods, b, req, opts );
}

int main(int argc, char** argv){
  return execopt(argc,argv)!=SCIP_OKAY ? 1:0;
}
