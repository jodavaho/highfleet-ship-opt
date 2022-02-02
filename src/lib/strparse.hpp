#ifndef strparse_hpp
#define strparse_hpp

#include "lib/hf-problem.hpp"

namespace hf
{
  enum PARSECODE{
    PARSE_OK=0,
    PARSE_INVARG,
  };

  PARSECODE hf_from_argc(const int argc, char** argv, hf::Bounds &bounds, hf::SolveOptions &opts,std::vector<module> &out_modules);

}

#endif
