#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

#include "lib/modules.hpp"
#include "opts.hpp"
#include "lib/hf-problem.hpp"
#include <cassert>
#include <cstring>
#include <iomanip>

void output_sol(std::vector<size_t> counts, std::vector<module> modules){
  size_t N = modules.size();
  assert(N==counts.size());
  size_t cost=0;
  for (size_t i=0;i<N;i++){
    if (counts[i]>0){
      size_t line_cost = counts[i]*modules[i].cost;
      cost+=line_cost;
      std::cout<<"+$"<<std::setw(8)<<line_cost;
      std::cout<<": "<<std::setw(2)<<counts[i]<<" x "<<modules[i].name<<std::endl;
    }
  }
  std::cout<<"----------"<<std::endl;
  std::cout<<"$"<<cost<<std::endl;
}

int execopt(int argc, char** argv){
  std::vector<module> available_mods;
  for (const auto m: get_all_modules()){
    available_mods.push_back(m);
  }
  std::vector<size_t> counts(available_mods.size());
  Bounds b;
  static struct {
    int operator()(module m1, module m2)const{
      return m1.name < m2.name;
    }
  }name_cmp;
  std::vector<module> req;
  parse_opts(argc, argv, b, req);
  auto begin = available_mods.begin();
  auto end = available_mods.end();
  for (auto m: req){
    auto it = std::find_if(
        available_mods.begin(),
        available_mods.end(),
        [&m](module &m2){return m2.name==m.name;}
        );
    if (it != end){
      counts[it-begin]++;
    }
  }
  SolveOptions opts;
  opts.include_hull=true;
  std::cout<<"Bounds:"<<std::endl<<b<<std::endl;
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
  if (argc>2 && strcmp(argv[1],"memtest")==0){
    for (int i=0;i<10;i++){
      execopt(argc-1,&argv[1]);
    }
    return 0;
  }
  if (argc>2 && strcmp(argv[1],"fill")==0){
    return execopt(argc-1, &argv[1]);
  }
  print_help(argc,argv);
  return 1;
}
