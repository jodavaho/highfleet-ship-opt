#ifndef MODULES_H
#define MODULES_H

#include <string>
#include <optional>
#include <vector>
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

//search among all known modules for one by name, return {} if none exists.
//(e.g., if ( auto x=by_name(...)) do_work(*x); )
std::optional<const module> by_name(std::string des);

//define output stream formatting, 
//
//e.g., std::cout<< *by_name("bridge") <<std::endl;
std::ostream& operator<<(std::ostream& os, const module&m);

//return all modules for iteration
//e.g.,. for (const auto m: get_all_modules()) {...}
std::vector<module> get_all_modules();

#endif
