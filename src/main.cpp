#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "lib/modules.hpp"
#include "opts.hpp"
#include "lib/hf-problem.hpp"
#include <cassert>
#include <cstring>

void output_sol(std::vector<size_t> counts, std::vector<module> modules){
  size_t N = modules.size();
  assert(N==counts.size());
  for (size_t i=0;i<N;i++){
    if (counts[i]>0){
      std::cout<<counts[i]<<" of "<<modules[i]<<std::endl;
    }
  }
}
int execopt(int argc, char** argv){
  std::vector<module> available_mods;
  for (const auto m: get_all_modules()){
    available_mods.push_back(m);
  }
  std::vector<size_t> counts;
  Bounds b;
  b.twr_max=5;
  b.twr_min=1;
  b.range_min=1000;
  b.spd_min=400;
  std::vector<module> req;

  for (int argi=0;argi<argc;argi++){
    auto req_mod = by_name(argv[argi]);
    if (req_mod){
      req.push_back(*req_mod);
    }
  }
  SolveOptions opts;
  opts.include_hull=true;
  SOLVECODE retcode = solve(counts, available_mods, b, req, opts );
  switch (retcode){
    case OK: {output_sol(counts, available_mods);return 0;}
    case ERR_INFEASIBLE:{ std::cout<<"Infeasible"<<std::endl; return 1;}
  }
  return 0;
}

void print_help(int argc, char**argv){
  std::cout<<argv[0]<<" <command>"<<std::endl;

}

int main(int argc, char** argv){
  if (argc>1 && strcmp(argv[1],"print-modules")==0){
    for (auto m: get_all_modules()){
      std::cout<<m<<std::endl;
    }
    return 0;
  }
  if (argc>2 && strcmp(argv[1],"fill")==0){
    for (int i=0;i<10;i++){
      return execopt(argc-2,&argv[2]);
    }
  }
  print_help(argc,argv);
  return 1;
}
