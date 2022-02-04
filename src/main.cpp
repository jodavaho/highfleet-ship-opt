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

using hf::Bounds;
using hf::SolveOptions;
using hf::solve;
using hf::SOLVECODE;

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
  std::vector<module> available_mods = hf::get_all_modules();
  assert(available_mods.size() == hf::num_modules());
  std::vector<size_t> in_out_counts(hf::num_modules(),0);
  Bounds b;
  std::vector<module> req;
  SolveOptions opts;
  if (!parse_opts(argc, argv, b, opts, req)){
    std::cerr<<"Cannot parse arguments. Try -h "<<std::endl;
    return -1;
  }
  for (auto m: req){
    for (size_t idx=0;idx<hf::num_modules();idx++){
      if (available_mods[idx].name == m.name){
        in_out_counts[idx]++;
      }
    }
  }
  opts.include_hull=true;
  std::cout<<"Bounds:"<<std::endl<<b<<std::endl;
  SOLVECODE retcode = solve(in_out_counts, available_mods, b, opts );
  switch (retcode){
    case hf::OK: {
                   std::cout<<"Solve successful!"<<std::endl;
                   output_sol(in_out_counts, available_mods);
                   return 0;}
    case hf::ERR_INFEASIBLE:{ std::cout<<"Infeasible"<<std::endl; return -1;}
    case hf::ERR_INTERNAL:{ std::cout<<"Internal hf library error!"<<std::endl; return -1;}
    case hf::ERR_INVARG:{ std::cout<<"Invalid arg passed in: Counts!=#mods"<<std::endl; return -1;}
  }
  return 0;
}

const char* docstring = 
"A Highfleet (tm) ship module selector and optimizer\n"
"(c) 2022 Joshua Vander Hook\n"
"Highfleet is trademarked by Konstantin Koshutin\n"
"\n";

const char* helpstr = 
"\n"
"There are a few subtleties to the solutions.\n"
" - Because of the way hull sections are priced,\n"
"   we may output many 1x1 hulls instead of 2x2 hulls\n"
"\n";

void print_help(int argc, char**argv){
  std::cout<<docstring<<std::endl;
  std::cout<<argv[0]<<" <command> [options] "<<std::endl;
  std::cout<<"   as in: "<<std::endl<<std::endl;
  std::cout<<argv[0]<<" help "<<std::endl;
  std::cout<<argv[0]<<" print-modules "<<std::endl;
  std::cout<<argv[0]<<" fill [bounds] [<module_tag>=<#>] "<<std::endl<<std::endl;
  std::cout<<" examples: "<<std::endl<<std::endl;
  std::cout<<" Generate something like the Lightning: "<<std::endl;
  std::cout<<argv[0]<<" fill --range_min=800 --speed_min=600 gun_ak100=2 fss=2 chassis_m=4 pod=2"<<std::endl;
}

int main(int argc, char** argv){
  if (argc>1 && strcmp(argv[1],"help")==0){
    print_help(argc,argv);
    return 0;
  }
  if (argc>1 && strcmp(argv[1],"print-modules")==0){
    for (auto m: hf::get_all_modules()){
      std::cout<<m<<std::endl;
    }
    std::cout<<"Also support the following aliases:"<<std::endl;
    return 0;
  }
  if (argc>2 && strcmp(argv[1],"memtest")==0){
    for (int i=0;i<10;i++){
      execopt(argc-2,&argv[2]);
    }
    return 0;
  }
  if (argc>2 && strcmp(argv[1],"fill")==0){
    return execopt(argc-1, &argv[1]);
  }
  print_help(argc,argv);
  return 1;
}
