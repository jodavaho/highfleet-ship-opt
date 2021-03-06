#ifndef MODULES_H
#define MODULES_H

#include <string>
#include <optional>
#include <vector>
#include <set>
#include <map>

namespace hf{

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
      RUNWAY,
      HULL
    }mount=SMALL;//default is small
    std::optional<module> get_hull() const;
  };

  //search among all known modules for one by name, return {} if none exists.
  //(e.g., if ( auto x=by_name(...)) do_work(*x); )
  std::optional<const module> by_name(std::string des);


  //define useful datastructure with name-based comparator. 
  struct namecmp{
    bool operator()(module a, module b)const{
      return a.name<b.name;
    }
  };
  using ModuleSet = std::set<module, namecmp>;

  //return all modules for iteration
  //e.g.,. for (const auto m: get_all_modules()) {...}
  const ModuleSet get_module_set();
  const std::vector<module> get_all_modules();
  const size_t num_modules();

  //a mapping of common_names->canonical names.
  //If the module X is also known as Y, then
  //aliases[Y] returns X (the "real" name)
  //AND therefore
  //get_module_set()[X] == get_module_set()[ aliases[Y] ]
  //The aliases are easier to remember, that's all. 
  const std::map<std::string, std::string>& get_aliases();

}

//define output stream formatting, 
//
//e.g., std::cout<< *by_name("bridge") <<std::endl;
std::ostream& operator<<(std::ostream& os, const hf::module&m);
#endif
