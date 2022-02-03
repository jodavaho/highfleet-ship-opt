#include "lib/strparse.hpp"
#include <iostream>

namespace hf{
  namespace parse{
    RETCODE from_argc(const int argc, char** argv, hf::Bounds &bounds, hf::SolveOptions &opts, std::vector<module> &out_modules)
    {
      for (int i=0;i<argc;i++)
      {
        std::string argval = std::string(argv[i]);

        //is a lower bound set as X=y?
        auto idx = argval.find('=');
        if (idx!=std::string::npos){
          //break up into name/count
          std::string name = argval.substr(0,idx);
          std::string val  = argval.substr(idx+1);
          auto m = hf::by_name(name);
          if (m){
            size_t count = std::atoi(val.c_str());
            for (size_t i=0;i<count;i++){
              out_modules.push_back(*m);
            }//count==0 falls through, and is not OK
          } else if (name=="range_min" || name=="range"){
            bounds.range_min=std::atof(val.c_str());
          } else if (name=="twr_min" || name=="twr"){
            bounds.twr_min=std::atof(val.c_str());
          } else if (name=="spd_min" || name=="spd"){
            bounds.spd_min=std::atof(val.c_str());
          } else if (argval=="quiet"){
            opts.quiet = (bool) std::atoi(val.c_str());
          } else {
            return INVKVARG;
          }
        } else {
          auto m = hf::by_name(argval);
          if (m){
            out_modules.push_back(*m);
          } else if (argval=="ignore_hull") {
            opts.include_hull=false;  
          } else if (argval=="quiet"){
            opts.quiet = true;
          } else {
            return INVKARG;
          }
        }
      }
      return OK;
    }
  }
}
