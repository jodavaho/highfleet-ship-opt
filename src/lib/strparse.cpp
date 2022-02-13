#include "lib/strparse.hpp"
#include <iostream>

namespace hf{
  namespace parse{

    RETCODE opts_from_argc(const int argc, char** argv, hf::SolveOptions&opts){
      for (int i=0;i<argc;i++)
      {
        std::string argval = std::string(argv[i]);

        //is a lower bound set as X=y?
        auto idx = argval.find('=');
        if (idx!=std::string::npos){
          //break up into name/count
          std::string name = argval.substr(0,idx);
          std::string val  = argval.substr(idx+1);
          if (argval=="quiet"){
            opts.quiet = (bool) std::atoi(val.c_str());
          } else {
            return INVKVARG;
          }
        } else if (argval=="ignore_hull") {
          opts.include_hull=false;  
        } else if (argval=="quiet"){
          opts.quiet = true;
        } else {
          return INVKARG;
        }
      }
      return OK;
    }

    RETCODE bounds_from_argc(const int argc, char** argv, hf::Bounds&bounds){
      for (int i=0;i<argc;i++)
      {
        std::string argval = std::string(argv[i]);
        //is a lower bound set as X=y?
        auto idx = argval.find('=');
        if (idx!=std::string::npos){
          //break up into name/count
          std::string name = argval.substr(0,idx);
          std::string val  = argval.substr(idx+1);
          if (name=="range_min" || name=="range"){
            bounds.range_min=std::atof(val.c_str());
          } else if (name=="twr_min" || name=="twr"){
            bounds.twr_min=std::atof(val.c_str());
          } else if (name=="spd_min" || name=="spd"){
            bounds.spd_min=std::atof(val.c_str());
          } else {
            return INVKVARG;
          }
        } else {
          return INVKARG;
        }
      }
      return OK;
    }

    RETCODE mods_from_argc(const int argc, char** argv, std::vector<module> &out_modules)
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
            if (count==0){
              //assume 1
              out_modules.push_back(*m);
            } else {
              for (size_t i=0;i<count;i++){
                out_modules.push_back(*m);
              }
            }
          } else {
            return INVMODNAME;
          }

        } else {
          auto m = hf::by_name(argval);
          if (m){
            out_modules.push_back(*m);
          } else {
            return INVMODNAME;
          }
        }
      }
      return OK;
    }

    RETCODE from_argc(const int argc, char** argv, hf::Bounds &bounds, hf::SolveOptions &opts, std::vector<module> &out_modules)
    {
      for (int i=0;i<argc;i++){
        if (OK==bounds_from_argc(1,&argv[i],bounds)){continue;}
        if (OK==opts_from_argc(1,&argv[i],opts)){continue;}
        if (OK==mods_from_argc(1,&argv[i],out_modules)){continue;}
        //problem!
        return BADARGV;
      }
      return OK;  
    }
  }
}
