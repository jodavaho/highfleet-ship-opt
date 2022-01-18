#ifndef MODULES_H
#define MODULES_H

#include <string>
#include <optional>
#include <set>

/**
 *
 * Generic, not very well ecapsulated, structure to hold all module data
 */
struct module{
  std::string name;
  double w;
  double h;
  double sq;
  double weight;
  double hp;
  double cost;
  double energy;
  double crew;
  double fuel_cap;
  double fuel_rate;
  double thrust;
  double ammo;
  double firepower;
  double fpl;
  enum mounting{
    SMALL,
    LARGE,
    EXTERIOR,
    HULL
  }mount=SMALL;//default is small
  std::optional<module> get_hull() const;
};


//define useful datastructure with name-based comparator. 
using ModuleSet = std::set<module, decltype([](const module&a, const module&b){return a.name<b.name;})>;

//search among all known modules for one by name, return {} if none exists.
//(e.g., if ( auto x=by_name(...)) do_work(*x); )
std::optional<const module> by_name(std::string des);

//return all modules for iteration
//e.g.,. for (const auto m: get_all_modules()) {...}
ModuleSet get_all_modules();

//define output stream formatting, 
//
//e.g., std::cout<< *by_name("bridge") <<std::endl;
std::ostream& operator<<(std::ostream& os, const module&m);

#endif
