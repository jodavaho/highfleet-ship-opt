#ifndef hf_problem_hpp
#define hf_problem_hpp

#include <vector>
#include "modules.hpp"

#ifndef hf_opt_version
#define hf_opt_version "0.9.0"
#endif

struct SolveOptions{
  bool include_hull=true;
  bool quiet=true;
};

struct Bounds{
  double cost_min=0;
  double cost_max=1e7;
  double wt_min=0;
  double wt_max=1e7;
  double spd_min=0;
  double spd_max=1e7;
  double range_min=0;
  double range_max=1e15;
  double twr_min=1.0;
  double twr_max=1e7;
  double combat_time_min=0;
  double combat_time_max=1e7;
  double pwr_ratio_min=1.0;
  double crew_ratio_min=1.0;
};

std::ostream& operator<<(std::ostream& os, const Bounds&b);

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
