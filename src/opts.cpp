#include "opts.hpp"
#include <getopt.h>
#include <iostream>
#include <cstring>

std::string get_help()
{
  return "";
}

void err_out(char* ret)
{
  std::cout<< "Unrecognized: "<<ret<<std::endl;
  std::cout<< get_help() << std::endl;
}

static const int RANGE_MIN=1;
static const int TWR_MIN=2;
static const int WT_MIN=3;
static const int WT_MAX=4;
static const int SPD_MIN=5;

bool parse_opts(int argc, char** argv, Bounds& out_bounds, std::vector<module> & out_modules)
{
  bool ok =false;
  static struct option long_opts[] = 
  {
    {"range_min", required_argument, 0, RANGE_MIN},
    {"twr_min", required_argument, 0, TWR_MIN},
    {"wt_min", required_argument, 0, WT_MIN},
    {"wt_max", required_argument, 0, WT_MAX},
    {"speed_min", required_argument, 0, SPD_MIN},
    {0,0,0,0}
  };

  int cur_idx;
  int opt_idx;
  int ret=0;
  while(1){
    ret = getopt_long(argc,argv,"",long_opts,&opt_idx);
    if (ret<0){break;}
    switch(ret){
      case      RANGE_MIN:  {ok=true;out_bounds.range_min=std::atof(optarg);  break;}
      case      TWR_MIN:    {ok=true;out_bounds.twr_min=std::atof(optarg);    break;}
      case      WT_MIN:     {ok=true;out_bounds.wt_min=std::atof(optarg);     break;}
      case      WT_MAX:     {ok=true;out_bounds.wt_max=std::atof(optarg);     break;}
      case      SPD_MIN:    {ok=true;out_bounds.spd_min=std::atof(optarg);    break;}
      default:  {err_out(optarg);return false;  break;}
    };
  }
  for (int i=optind;i<argc;i++)
  {
    std::string argval = std::string(argv[i]);
    auto idx = argval.find('=');
    if (idx!=std::string::npos){
      //break up into name/count
      std::string name = argval.substr(0,idx);
      auto m = by_name(name);
      if (m){
        std::string val  = argval.substr(idx+1);
        size_t count = std::atoi(val.c_str());
        for (int i=0;i<count;i++){
          out_modules.push_back(*m);
          ok=true;
        }//count==0 falls through, and is not OK
      } else {
        std::cerr<<"Unrecognized argument: "<<argval<<std::endl;
        return false;
      }
    } else {
      auto m = by_name(argval);
      if (!m){
        std::cerr<<"Unrecognized argument: "<<argval<<std::endl;
        return false;
      } else {
        //got a valid module request
        out_modules.push_back(*m);
        ok=true;
      }
    }
  }
  return false;
}

std::string get_credits()
{
  return "";
}
