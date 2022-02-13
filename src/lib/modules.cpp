#include "modules.hpp"

#include <iostream>
#include <string>
#include <optional>
#include <sstream>
#include <map>

using hf::module;

namespace module_helpers{
  static std::optional<module> get_hull(const double sqs,const module::mounting mount)
  {
    switch(mount){
      case module::LARGE:{ return hf::by_name("hull_4x4"); }
      case module::EXTERIOR:{ return {}; }
      case module::RUNWAY:{ return hf::by_name("flight_deck"); }
      case module::HULL:{ return {}; }
      case module::SMALL:{ 
                           if (sqs==2.0){
                             return hf::by_name("hull_2x1");
                           } else if (sqs==1.0){
                             return hf::by_name("hull_1x1");
                           } else if (sqs==4.0){
                             return hf::by_name("hull_2x2");
                           } else {
                             return {};
                           }
                         }
    };
    return {};
  }
}

namespace hf{

  static const std::map<std::string, std::string> aliases =
  {
    {"pod","sk_2m"},
    {"escape_pod","sk_2m"},
    {"fss","ec13"},
    {"mars","irst"},
    {"fcr_l","mr_12"},
    {"fcr","mr_12"},
    {"fcr_s","mr_2m"},
    {"sr_l","mr_700"},
    {"radar_l","mr_700"},
    {"radar","mr_700"},
    {"sr_s","mr_500"},
    {"radar_s","mr_500"},
    {"elint_l","mp_21"},
    {"elint","mp_21"},
    {"elint_s","mp_12"},

    {"37mm","gun_2a37"},
    {"2a37","gun_2a37"},

    {"57mm","gun_ak725"},
    {"ak725","gun_ak725"},

    {"100mm","gun_ak100"},
    {"ak100","gun_ak100"},

    {"130mm","gun_d80"},
    {"d80","gun_d80"},
    {"molot","gun_d80"},

    {"180mm","gun_1x180"},
    {"mk1","gun_1x180"},
    {"2x180mm","gun_2x180"},
    {"mk2","gun_2x180"},
    {"6x180mm","gun_6x180"},
    {"mk6","gun_6x180"},
  };

  const std::map<std::string, std::string>& get_aliases(){
    return aliases;
  }

  static const ModuleSet all_modules = 
  {
    {"bridge",             2,  2,  4,   25,     100,  0,      0,      60,   0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"hull_2x2",           2,  2,  4,   25.8,   100,  20,     0,      0,    0,    0,     0,     0,   0,   0,      module::HULL},
    //{"hull_2x2_ic",        2,  2,  4,   25.8,   100,  20,     0,      0,    0,    0,     0,     0,   0,   0,      module::HULL},
    //{"hull_2x2_a",         2,  2,  4,   25.8,   100,  20,     0,      0,    0,    0,     0,     0,   0,   0,      module::HULL},
    //{"hull_2x2_triangle",  2,  2,  4,   25.8,   100,  20,     0,      0,    0,    0,     0,     0,   0,   0,      module::HULL},
    {"hull_2x1",           2,  1,  2,   11.2,   40,   10,     0,      0,    0,    0,     0,     0,   0,   0,      module::HULL},
    {"hull_1x1",           1,  1,  1,   5.6,    25,   5,      0,      0,    0,    0,     0,     0,   0,   0,      module::HULL},
    //{"hull_1x1_corner",    1,  1,  1,   5.6,    25,   5,      0,      0,    0,    0,     0,     0,   0,   0,      module::HULL},
    {"hull_4x4",           4,  4,  16,  574.2,  400,  80,     0,      0,    0,    0,     0,     0,   0,   0,      module::HULL},
    {"flight_deck",        2,  1,  2,   11.8,   100,  100,    0,      0,    0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"armor_small",        1,  1,  1,   64.3,   100,  200,    0,      0,    0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"armor_large",        2,  1,  2,   128.6,  200,  400,    0,      0,    0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"fuel_small",         2,  1,  2,   37.3,   30,   10,     0,      0,    40,   0,     0,     0,   0,   0,      module::SMALL},
    {"fuel_large",         4,  4,  16,  430.6,  400,  80,     0,      0,    450,  0,     0,     0,   0,   0,      module::LARGE},
    {"ammo_small",         2,  1,  2,   107.2,  20,   500,    -0.8,   -5,   0,    0,     0,     1,   0,   0,      module::SMALL},
    {"ammo_large",         2,  2,  4,   197.2,  20,   1000,   -1.6,   -10,  0,    0,     0,     2,   0,   0,      module::SMALL},
    {"eng_d30",            2,  2,  4,   22.5,   100,  1000,   -1.2,   -4,   0,    -3,    12.5,  0,   0,   0,      module::SMALL},
    {"eng_nk25",           2,  2,  4,   30,     100,  1500,   -1.3,   -4,   0,    -6.4,  18,    0,   0,   0,      module::SMALL},
    {"eng_d30s",           2,  2,  4,   18.6,   100,  1000,   -0.2,   -4,   0,    -4,    21,    0,   0,   0,      module::SMALL},
    {"eng_rd59",           4,  4,  16,  300.7,  400,  2000,   -2,     -8,   0,    -7,    45,    0,   0,   0,      module::LARGE},
    {"eng_rd51",           4,  4,  16,  214.5,  400,  2000,   -1,     -8,   0,    -7,    65,    0,   0,   0,      module::LARGE},
    {"chassis_s",          0,  0,  0,   2.8,    50,   50,     -0.05,  0,    0,    0,     0,     0,   0,   0,      module::SMALL},
    {"chassis_m",          0,  0,  0,   17.9,   80,   100,    -0.1,   0,    0,    0,     0,     0,   0,   0,      module::SMALL},
    {"chassis_l",          0,  0,  0,   79.4,   100,  200,    -0.2,   0,    0,    0,     0,     0,   0,   0,      module::SMALL},
    {"chassis_x",          0,  0,  0,   237.6,  200,  400,    -0.4,   0,    0,    0,     0,     0,   0,   0,      module::SMALL},
    {"gun_2a37",           2,  2,  4,   51.1,   100,  3000,   -0.75,  -6,   0,    0,     0,     -1,  2,   0.75,   module::SMALL},
    {"gun_ak725",          2,  2,  4,   51.1,   100,  1500,   -0.75,  -6,   0,    0,     0,     -1,  2,   2.8,    module::SMALL},
    {"gun_ak100",          2,  2,  4,   51.1,   100,  2000,   -1,     -6,   0,    0,     0,     -1,  2,   8,      module::SMALL},
    {"gun_d80",            2,  2,  4,   51.1,   100,  4000,   -1,     -6,   0,    0,     0,     -1,  2,   8,      module::SMALL},
    {"gun_1x180",          2,  2,  4,   81.2,   100,  4000,   -1.8,   -12,  0,    0,     0,     -2,  4,   13.66,  module::SMALL},
    {"gun_2x180",          2,  2,  4,   81.2,   100,  6000,   -2.4,   -24,  0,    0,     0,     -4,  8,   16.4,   module::SMALL},
    {"gun_6x180",          4,  4,  16,  409.2,  200,  24000,  -6,     -50,  0,    0,     0,     -8,  16,  70.28,  module::LARGE},
    {"gun_a220",           2,  2,  4,   51.1,   100,  4000,   -3,     -5,   0,    0,     0,     -4,  8,   27,     module::SMALL},
    {"gen_mgu61",          2,  2,  4,   93.2,   40,   200,    6.1,    -5,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"gen_mgu28",          2,  1,  2,   43.4,   80,   150,    2.8,    -2,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"crew_large",         2,  2,  4,   35.3,   200,  200,    -0.5,   50,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"crew_small",         2,  1,  2,   43.3,   100,  100,    -0.2,   25,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"mis_zenith",         1,  2,  2,   6.3,    20,   600,    0,      -2,   0,    0,     0,     0,   1,   0,      module::SMALL},
    {"mis_sprint",         1,  1,  1,   4.6,    100,  300,    0,      -2,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"bomb_fab1000",       2,  1,  2,   37.3,   100,  200,    0,      0,    0,    0,     0,     0,   1,   0,      module::SMALL},
    {"mapmissile",         2,  4,  8,   261.1,  20,   1500,   0,      -5,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"mapmissilenuclear",  2,  4,  8,   261.1,  20,   4000,   0,      -5,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"air_la29",           3,  1,  3,   39.1,   100,  2000,   0,      -15,  0,    0,     0,     -1,  0,   0,      module::RUNWAY},
    {"air_t7",             4,  2,  8,   107.8,  100,  2500,   0,      -15,  0,    0,     0,     -1,  0,   0,      module::RUNWAY},
    {"mr_700",             4,  3,  12,  197.8,  100,  10000,  -6,     -2,   0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"mr_500",             1,  2,  2,   14.4,   100,  4000,   -2,     -4,   0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"mp_404",             2,  3,  6,   44.2,   100,  4000,   -2,     -1,   0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"mp_21",              1,  1,  1,   8.5,    100,  2000,   -2,     -2,   0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"mr_12",              2,  2,  4,   189.6,  0,    3000,   -2,     -1,   0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"mr_2m",              1,  1,  1,   7.5,    0,    1000,   -1,     -2,   0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"irst",               2,  2,  4,   12.9,   100,  2000,   -1,     -1,   0,    0,     0,     0,   0,   0,      module::EXTERIOR},
    {"jammer",             2,  1,  2,   23.3,   30,   2000,   -1,     -1,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"flares",             1,  1,  1,   4.2,    40,   800,    0,      0,    0,    0,     0,     0,   0,   0,      module::SMALL},
    {"palash",             1,  1,  1,   4.2,    100,  1500,   0,      0,    0,    0,     0,     0,   0,   0,      module::SMALL},
    {"ec13",               2,  1,  2,   31.3,   10,   300,    0,      -2,   0,    0,     0,     0,   0,   0,      module::SMALL},
    {"sk_2m",              1,  1,  1,   17.9,   20,   100,    0,      0,    0,    0,     0,     0,   0,   0,      module::SMALL},
  };

  const std::vector<module> vec_modules(all_modules.begin(), all_modules.end());
  const size_t num_modules(){ return all_modules.size(); }

  const std::vector<module> get_all_modules()
  {
    return vec_modules;
  }

  std::optional<const module> by_name(std::string des)
  {
    std::string lname;
    //might be a "slang name"
    auto name_itr = aliases.find(des);
    if (name_itr !=aliases.end()){
      //get "real" name
      lname = name_itr->second;
    } else {
      //assume this *is* the "real" name
      lname = des;
    }
    //got a name, perhaps not one we know ... 
    module dummy{.name=lname};
    auto it = all_modules.find(dummy);
    if (it!=all_modules.end()){
      return *it;
    }
    //couldn't find module by that name
    return {};
  }


  std::optional<module> module::get_hull() const
  {
    return module_helpers::get_hull(sq,mount);
  }

}

std::ostream& operator<<(std::ostream& os, const module&m){
  os<<m.name<<" ";
  os<<"$"<<m.cost<<" ";
  os<<m.weight<<"T ";
  os<<m.thrust<<"MN ";
  os<<m.fuel_cap<<"Tf ";
  os<<m.fuel_rate<<"kg/s ";
  os<<m.ammo<<"ammo ";
  os<<m.crew<<"crew ";
  os<<m.energy<<"MW ";
  return os;
}

