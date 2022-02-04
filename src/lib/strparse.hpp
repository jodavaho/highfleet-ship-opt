#ifndef strparse_hpp
#define strparse_hpp

#include "lib/hf-problem.hpp"

namespace hf
{
  namespace parse{
    enum RETCODE{
      OK=0,
      INVKARG,
      INVKVARG,
      INVMODNAME,
      BADARGV
    };

    RETCODE mods_from_argc(const int argc, char** argv, std::vector<module> &);
    RETCODE opts_from_argc(const int argc, char** argv, hf::SolveOptions &);
    RETCODE bounds_from_argc(const int argc, char** argv, hf::Bounds &);
    RETCODE from_argc(const int argc, char** argv, hf::Bounds &bounds, hf::SolveOptions &opts,std::vector<module> &out_modules);

  }
}

#endif
