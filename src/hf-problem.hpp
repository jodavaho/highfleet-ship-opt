#ifndef hf_problem_hpp
#define hf_problem_hpp

#include <unordered_map>
#include <sstream>
#include "modules.hpp"

struct SolveOptions{
  bool include_hull=true;
};

struct Bounds{
  double cost[2]={0,1e7};
  double weight[2]={0,1e7};
  double speed[2]={0,1e7};
  double range[2]={0,1e15};
  double twr[2]={1.0,1e7};
  double combat_time[2]={0,1e7};
  double min_pwr_ratio=1.0;
  double min_crew_ratio=1.0;
};

enum SOLVECODE{
  OK=0,
  ERR_INFEASIBLE,
  ERR_INTERNAL
};

SOLVECODE solve(
    std::vector<size_t> &out_counts,
    const std::vector<module> &mods,
    const Bounds bounds=Bounds(),
    const std::vector<module> required={} ,
    const SolveOptions opts={}
    );


#endif
