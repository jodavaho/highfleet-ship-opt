#include <fstream>
#include <iostream>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "modules.hpp"

SCIP* g = NULL;
std::vector<SCIP_VAR*> vars;
std::vector<SCIP_EXPR*> ex_vars;
SCIP_SOL * solution = NULL;
SCIP_CONSHDLR * conshdlr_nl;

struct SCIPLOCK{
  SCIPLOCK(){}
  ~SCIPLOCK(){
    for (auto ptr: ex_vars){
      SCIPreleaseExpr(g,&ptr);
    }
    for (auto ptr: vars){
      SCIPreleaseVar(g,&ptr);
    }
    if(g){SCIPfree(&g);}
  }
};

struct Bounds{
  double cost[2]={0,INFINITY};
  double weight[2]={0,INFINITY};
  double speed[2]={0,INFINITY};
  double range[2]={0,INFINITY};
  double twr[2]={1.0,INFINITY};
  double combat_time[2]={0,INFINITY};
};

int solve(
    std::vector<size_t> &out_counts,
    const std::vector<module> &mods,
    const Bounds bounds=Bounds(),
    const std::vector<module> required={} 
    )
{

  SCIPLOCK lock;//RAII-ish
  SCIP_CALL( SCIPcreate(&g));
  SCIP_CALL( SCIPincludeDefaultPlugins(g));
  SCIP_CALL( SCIPcreateProbBasic(g, "Highfleet_Component_Selection"));
  SCIP_CALL( SCIPsetObjsense(g, SCIP_OBJSENSE_MINIMIZE));
  size_t N = mods.size();
  vars.reserve(N);
  ex_vars.reserve(N);

  //tally up the required modules first
  std::unordered_map<std::string,size_t> mod_minimums;
  for (const auto &mod: mods){
    mod_minimums[mod.name]=0;
  }
  for (const auto &req: required){
    mod_minimums[req.name]++;
  }
 
  //create optimization variables for n_<mod> for each mod
  for (size_t i=0;i<N;i++){
    auto m = mods[i];
    std::stringstream namss;
    namss<<"n_"<<m.name;

    std::string nam = namss.str();
    auto varp = &vars[i];
    //create var for count
    size_t min = mod_minimums[m.name];
    assert(min>=0);
    SCIP_CALL ( SCIPcreateVarBasic(g,varp, nam.c_str(), min, SCIPinfinity(g), 1.0, SCIP_VARTYPE_INTEGER) ) ;
    SCIP_CALL ( SCIPaddVar(g,*varp) );
    auto ex_varp = &ex_vars[i];
    //create xprvar for count
    SCIP_CALL ( SCIPcreateExprVar(g,ex_varp,*varp,NULL,NULL) );
    std::cout<<"Created: "<<nam<<std::endl;
  }

  //create the sum_of(resource) for each resource

  SCIP_EXPR *sum_fuel_cap=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].fuel_cap;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_fuel_cap,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_fuel_rate=nullptr;
  SCIP_EXPR *sum_fuel_rate_3p6=nullptr;
  SCIP_EXPR *sum_fuel_rate_50=nullptr;
  SCIP_EXPR *inv_sum_fuel_rate_3p6=nullptr;
  SCIP_EXPR *inv_sum_fuel_rate_50=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].fuel_rate;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_fuel_rate,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
    //later need 1/(3.6 x)
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_fuel_rate_3p6,N,ex_vars.data(),vals.data(),3.6,NULL,NULL));
    SCIP_CALL ( SCIPcreateExprSignpower(g,&inv_sum_fuel_rate_3p6,sum_fuel_rate_3p6,-1.0,NULL,NULL));
    //later need 1/(50 x)
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_fuel_rate_50,N,ex_vars.data(),vals.data(),50.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateExprSignpower(g,&inv_sum_fuel_rate_50,sum_fuel_rate_50,-1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_thrust=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].thrust;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_thrust,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }
   
  SCIP_EXPR *sum_cost=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].cost;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_cost,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_weight=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].weight;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_weight,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_crew=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].crew;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_crew,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }
  
  SCIP_EXPR *sum_energy=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].energy;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_energy,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_firepower=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].firepower;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_firepower,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }

  //via /u/jodavaho
  //speed = 90.0*twr
  SCIP_EXPR * twr = nullptr;
  SCIP_EXPR *exp_speed =nullptr;
  {
    SCIP_EXPR *inv_wt, *terms[2];
    SCIP_CALL ( SCIPcreateExprSignpower(g,&inv_wt,sum_weight,-1.0,NULL,NULL));
    terms[0]=sum_thrust;
    terms[1]=inv_wt;
    SCIP_CALL ( SCIPcreateExprProduct(g,&twr,2,terms,1.0,NULL,NULL));
    terms[0]=twr;
    SCIP_CALL ( SCIPcreateExprProduct(g,&exp_speed,1,terms,90.0,NULL,NULL));
  }

  //via /u/NoamChomskyForever
  //Range = Fuel Capacity * 1000 / Total Fuel Needed / 3600 * Speed
  //Range = sum_fuel_cap / (3.6 sum_fuel_rate) * speed
  SCIP_EXPR *exp_range=nullptr;
  {
    SCIP_EXPR* terms[3]={sum_fuel_cap,inv_sum_fuel_rate_3p6,exp_speed};
    SCIP_CALL ( SCIPcreateExprProduct(g,&exp_range,3,terms,1.0,NULL,NULL));
  }

  //via /u/d0d0b1rd
  //Combat Time: sum(fuel_cap) / (sum(fuel_rate)*50) in seconds
  SCIP_EXPR * exp_combat_time = nullptr;
  {
    SCIP_EXPR* terms[2]= {sum_fuel_cap, inv_sum_fuel_rate_50};
    SCIP_CALL( SCIPcreateExprProduct(g,&exp_combat_time,2,terms,1.0,NULL,NULL));
  }

  //add minimum constraints
  SCIP_CONS *minimum_twr_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&minimum_twr_cons, "TWR_Bounsd" ,twr, bounds.twr[0], bounds.twr[1]));
  SCIP_CALL ( SCIPaddCons(g,minimum_twr_cons) );

  SCIP_CONS* maximum_cost_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&maximum_cost_cons, "Maximum_cost" ,sum_cost, bounds.cost[0], bounds.cost[1]));
  SCIP_CALL ( SCIPaddCons(g,maximum_cost_cons) );
 
  SCIP_CONS* minimum_range_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&minimum_range_cons, "Minimum_Range" ,exp_range,bounds.range[0],bounds.range[1]));
  SCIP_CALL ( SCIPaddCons(g,minimum_range_cons) );
    
  SCIP_CONS* minimum_speed_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&minimum_speed_cons, "Minumum_Speed" ,exp_speed, bounds.speed[0], bounds.speed[1]));
  SCIP_CALL ( SCIPaddCons(g,minimum_speed_cons) );

  SCIP_CONS* maximum_weight_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&maximum_weight_cons, "Maximum_Weight", sum_weight,bounds.weight[0], bounds.weight[1]) );
  SCIP_CALL ( SCIPaddCons(g,maximum_weight_cons));
  
  SCIP_CONS* minimum_combat_time_cons;
  SCIP_CALL( SCIPcreateConsBasicNonlinear(g,&minimum_combat_time_cons,"Minimum Combat Time", exp_combat_time, bounds.combat_time[0], bounds.combat_time[1]) );
  SCIP_CALL( SCIPaddCons(g,minimum_combat_time_cons));

  SCIP_CALL( SCIPprintOrigProblem(g, NULL, "cip", FALSE) ); 
  SCIP_CALL ( SCIPsolve(g) );
  if( SCIPgetNSols(g) > 0 )
  {
    SCIPinfoMessage(g, NULL, "\nSolution:\n");
    SCIP_CALL( SCIPprintSol(g, SCIPgetBestSol(g), NULL, FALSE) );
  }

  return SCIP_OKAY;
}

int execopt(int argc, char** argv){
  ModuleSet all;
  std::vector<size_t> counts;
  std::vector<module> available_mods;
  for (auto m: all_modules){
    std::cout<<"Adding: "<<m.name<<std::endl;
    available_mods.push_back(m);
  }
  Bounds b;
  b.speed[0]=400;
  std::vector<module> req;
  req.push_back( *by_name("d-80") );
  return solve(counts, available_mods, b );
}

int main(int argc, char** argv){
 
  return execopt(argc,argv)!=SCIP_OKAY ? 1:0;
}
