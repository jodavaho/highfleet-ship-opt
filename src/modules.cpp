#include "modules.hpp"

#include <iostream>
#include <string>
#include <optional>
#include <sstream>

namespace module_helpers{
  static std::optional<module> get_hull(const double sqs,const module::mounting mount)
  {
    switch(mount){
      case module::LARGE:{ return by_name("largehull"); }
      case module::EXTERIOR:{ return {}; }
      case module::SMALL:{ 
                           if (sqs==2.0){
                             return by_name("2x1hull");
                           } else if (sqs==1.0){
                             return by_name("smallhull");
                           } else {
                             return by_name("hull");
                           }
                         }
    };
    return {};
  }
};

std::optional<const module> by_name(std::string des)
{
  for (const auto m: get_all_modules()){
    if (m.name==des){
      return m;
    }
  }
  return std::optional<module>();
}


std::optional<module> module::get_hull() const
{
  return module_helpers::get_hull(sq,mount);
}

ModuleSet create_all_mods(){
  ModuleSet ret;
  ret.insert({"bridge",              2,  2,  4,   25,     100,   0,     0,     60,    0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"hull",                2,  2,  4,   25.8,   100,  20,     0,      0,    0,    0,     0,     0,   0,   0,    module::HULL});
  ret.insert({"hulltriangle",        2,  2,  4,   25.8,   100,  20,     0,      0,    0,    0,     0,     0,   0,   0,    module::HULL});
  ret.insert({"hullinvertedcorner",  2,  2,  4,   25.8,   100,  20,     0,      0,    0,    0,     0,     0,   0,   0,    module::HULL});
  ret.insert({"hullAshape",          2,  2,  4,   25.8,   100,  20,     0,      0,    0,    0,     0,     0,   0,   0,    module::HULL});
  ret.insert({"2x1hull",             2,  1,  2,   11.2,   40,   10,     0,      0,    0,    0,     0,     0,   0,   0,    module::HULL});
  ret.insert({"smallhull",           1,  1,  1,   5.6,    25,   5,      0,      0,    0,    0,     0,     0,   0,   0,    module::HULL});
  ret.insert({"smallhullcorner",     1,  1,  1,   5.6,    25,   5,      0,      0,    0,    0,     0,     0,   0,   0,    module::HULL});
  ret.insert({"largehull",           4,  4,  16,  574.2,  400,  80,     0,      0,    0,    0,     0,     0,   0,   0,    module::HULL});
  ret.insert({"FlightDeck",          2,  1,  2,   11.8,   100,  100,    0,      0,    0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"Armor",               1,  1,  1,   64.3,   100,  200,    0,      0,    0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"Largearmor",          2,  1,  2,   128.6,  200,  400,    0,      0,    0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"FuelSmall",           2,  1,  2,   37.3,   30,   10,     0,      0,    40,   0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"FuelLarge",           4,  4,  16,  430.6,  400,  80,     0,      0,    450,  0,     0,     0,   0,   0,    module::LARGE});
  ret.insert({"AmmoSmall",           2,  1,  2,   107.2,  20,   500,    -0.8,   -5,   0,    0,     0,     1,   0,   0,    module::SMALL});
  ret.insert({"AmmoLarge",           2,  2,  4,   197.2,  20,   1000,   -1.6,   -10,  0,    0,     0,     2,   0,   0,    module::SMALL});
  ret.insert({"D_30",                2,  2,  4,   22.5,   100,  1000,   -1.2,   -4,   0,    -3,    12.5,  0,   0,   0,    module::SMALL});
  ret.insert({"NK_25",               2,  2,  4,   30,     100,  1500,   -1.3,   -4,   0,    -6.4,  18,    0,   0,   0,    module::SMALL});
  ret.insert({"D_30S",               2,  2,  4,   18.6,   100,  1000,   -0.2,   -4,   0,    -4,    21,    0,   0,   0,    module::SMALL});
  ret.insert({"RD_59",               4,  4,  16,  300.7,  400,  2000,   -2,     -8,   0,    -7,    45,    0,   0,   0,    module::LARGE});
  ret.insert({"RD_51",               4,  4,  16,  214.5,  400,  2000,   -1,     -8,   0,    -7,    65,    0,   0,   0,    module::LARGE});
  ret.insert({"ChassisA",            0,  0,  0,   2.8,    50,   50,     -0.05,  0,    0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"ChassisB",            0,  0,  0,   17.9,   80,   100,    -0.1,   0,    0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"ChassisC",            0,  0,  0,   79.4,   100,  200,    -0.2,   0,    0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"ChassisD",            0,  0,  0,   237.6,  200,  400,    -0.4,   0,    0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"2a37",                2,  2,  4,   51.1,   100,  3000,   -0.75,  -6,   0,    0,     0,     -1,  2,   0.75, module::SMALL});
  ret.insert({"ak_725",              2,  2,  4,   51.1,   100,  1500,   -0.75,  -6,   0,    0,     0,     -1,  2,   2.8,  module::SMALL});
  ret.insert({"ak_100",              2,  2,  4,   51.1,   100,  2000,   -1,     -6,   0,    0,     0,     -1,  2,   8,    module::SMALL});
  ret.insert({"d_80",                2,  2,  4,   51.1,   100,  4000,   -1,     -6,   0,    0,     0,     -1,  2,   8,    module::SMALL});
  ret.insert({"mk_1_180",            2,  2,  4,   81.2,   100,  4000,   -1.8,   -12,  0,    0,     0,     -2,  4,   13.66,module::SMALL});
  ret.insert({"mk_2_180",            2,  2,  4,   81.2,   100,  6000,   -2.4,   -24,  0,    0,     0,     -4,  8,   16.4, module::SMALL});
  ret.insert({"mk_6_180",            4,  4,  16,  409.2,  200,  24000,  -6,     -50,  0,    0,     0,     -8,  16,  70.28,module::LARGE});
  ret.insert({"a_220",               2,  2,  4,   51.1,   100,  4000,   -3,     -5,   0,    0,     0,     -4,  8,   27,   module::SMALL});
  ret.insert({"mgu_61",              2,  2,  4,   93.2,   40,   200,    6.1,    -5,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"mgu_28",              2,  1,  2,   43.4,   80,   150,    2.8,    -2,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"CrewLarge",           2,  2,  4,   35.3,   200,  200,    -0.5,   50,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"CrewSmall",           2,  1,  2,   43.3,   100,  100,    -0.2,   25,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"Zenith",              1,  2,  2,   6.3,    20,   600,    0,      -2,   0,    0,     0,     0,   1,   0,    module::SMALL});
  ret.insert({"Sprint",              1,  1,  1,   4.6,    100,  300,    0,      -2,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"FAB_1000",            2,  1,  2,   37.3,   100,  200,    0,      0,    0,    0,     0,     0,   1,   0,    module::SMALL});
  ret.insert({"MapMissile",          2,  4,  8,   261.1,  20,   1500,   0,      -5,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"MapMissilenuclear",   2,  4,  8,   261.1,  20,   4000,   0,      -5,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"LA_29",               3,  1,  3,   39.1,   100,  2000,   0,      -15,  0,    0,     0,     -1,  0,   0,    module::SMALL});
  ret.insert({"T_7",                 4,  2,  8,   107.8,  100,  2500,   0,      -15,  0,    0,     0,     -1,  0,   0,    module::SMALL});
  ret.insert({"MR_700",              4,  3,  12,  197.8,  100,  10000,  -6,     -2,   0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"MR_500",              1,  2,  2,   14.4,   100,  4000,   -2,     -4,   0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"MP_404",              2,  3,  6,   44.2,   100,  4000,   -2,     -1,   0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"MP_21",               1,  1,  1,   8.5,    100,  2000,   -2,     -2,   0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"MR_12",               2,  2,  4,   189.6,  0,    3000,   -2,     -1,   0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"MR_2M",               1,  1,  1,   7.5,    0,    1000,   -1,     -2,   0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"IRS_1",               2,  2,  4,   12.9,   100,  2000,   -1,     -1,   0,    0,     0,     0,   0,   0,    module::EXTERIOR});
  ret.insert({"Jammer",              2,  1,  2,   23.3,   30,   2000,   -1,     -1,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"ASO_75Flares",        1,  1,  1,   4.2,    40,   800,    0,      0,    0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"Palash_1",            1,  1,  1,   4.2,    100,  1500,   0,      0,    0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"EC13",                2,  1,  2,   31.3,   10,   300,    0,      -2,   0,    0,     0,     0,   0,   0,    module::SMALL});
  ret.insert({"SK_2M",               1,  1,  1,   17.9,   20,   100,    0,      0,    0,    0,     0,     0,   0,   0,    module::SMALL});
  return ret;
}

const ModuleSet all_modules = create_all_mods();

ModuleSet get_all_modules()
{
  return all_modules;
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
