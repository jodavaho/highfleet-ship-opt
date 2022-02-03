#include "opts.hpp"
#include <getopt.h>
#include <iostream>
#include <cstring>
#include "lib/strparse.hpp"

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

bool parse_opts(int argc, char** argv, Bounds& out_bounds, SolveOptions & out_opts, std::vector<module> & out_modules)
{
  static struct option long_opts[] = 
  {
    {"range_min", required_argument, 0, RANGE_MIN},
    {"range", required_argument, 0, RANGE_MIN},
    {"twr_min", required_argument, 0, TWR_MIN},
    {"twr", required_argument, 0, TWR_MIN},
    {"speed_min", required_argument, 0, SPD_MIN},
    {"speed", required_argument, 0, SPD_MIN},
    {"wt_min", required_argument, 0, WT_MIN},
    {"wt_max", required_argument, 0, WT_MAX},
    {0,0,0,0}
  };

  int opt_idx;
  int ret=0;
  while(1){
    ret = getopt_long(argc,argv,"",long_opts,&opt_idx);
    if (ret<0){break;}
    switch(ret){
      case      RANGE_MIN:  {out_bounds.range_min=std::atof(optarg);  break;}
      case      TWR_MIN:    {out_bounds.twr_min=std::atof(optarg);    break;}
      case      WT_MIN:     {out_bounds.wt_min=std::atof(optarg);     break;}
      case      WT_MAX:     {out_bounds.wt_max=std::atof(optarg);     break;}
      case      SPD_MIN:    {out_bounds.spd_min=std::atof(optarg);    break;}
      default:  {err_out(optarg);return false;  break;}
    };
  }
  for (int i=optind;i<argc;i++)
  {
    switch (hf::parse::from_argc(1,&argv[i],out_bounds,out_opts,out_modules)){
      case hf::parse::OK:{continue;}
      case hf::parse::INVKVARG: { std::cerr<<"Unrecognized key-value argument: "<<argv[i]<<std::endl; return false; }
      case hf::parse::INVKARG:{ std::cerr<<"Unrecognized key-value argument: "<<argv[i]<<std::endl; return false;} 
      case hf::parse::INVMODNAME:{ std::cerr<<"Unrecognized mod name: "<<argv[i]<<std::endl; return false;} 
      case hf::parse::BADARGV:{ std::cerr<<"Unparseable string: "<<argv[i]<<std::endl; return false;}

    }
  }
  return true;
}

std::string get_credits()
{
  return "";
}
