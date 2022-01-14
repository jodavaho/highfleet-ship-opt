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

int solve(
    std::vector<size_t> &out_counts,
    const std::vector<module> &mods,
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
  }

  //create the sum_of(resource) for each resource

  SCIP_EXPR *sum_fuel_cap=nullptr;
  {
    std::vector<double> vals(N);
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
    std::vector<double> vals(N);
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
    std::vector<double> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].thrust;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_thrust,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }
   
  SCIP_EXPR *sum_cost=nullptr;
  {
    std::vector<double> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].cost;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_cost,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_weight=nullptr;
  {
    std::vector<double> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].weight;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_weight,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_crew=nullptr;
  {
    std::vector<double> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].crew;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_crew,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }
  
  SCIP_EXPR *sum_energy=nullptr;
  {
    std::vector<double> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].energy;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_energy,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_firepower=nullptr;
  {
    std::vector<double> vals(N);
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
    SCIP_EXPR *inv_wt, *terms[2]={inv_wt,sum_thrust};
    SCIP_CALL ( SCIPcreateExprSignpower(g,&inv_wt,sum_weight,-1.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateExprProduct(g,&twr,2,terms,1.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateExprProduct(g,&exp_speed,1,&twr,90.0,NULL,NULL));
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


#define CONS_DEFAULT 1, 1, 1, 1, 1, 0, 0, 0, 0
  //add minimum constraints
  //T/W >= desired
  {
    SCIP_CONS *minimum_twr_cons;
    double minimum_twr_value = 1.0;
    //SCIP_CALL ( SCIPcreateConsNonlinear(g,&minimum_twr_cons, "Minimum_TWR" ,twr, minimum_twr_value, SCIPinfinity(g), CONS_DEFAULT));
    //SCIP_CALL ( SCIPaddCons(g,minimum_twr_cons) );
  }

  //cost =< desired
  SCIP_CONS* maximum_cost_cons;
  double maximum_cost_value=300000.0;
  SCIP_CALL ( SCIPcreateConsNonlinear(g,&maximum_cost_cons, "Maximum_cost" ,sum_cost, 0.0, maximum_cost_value, CONS_DEFAULT));
  SCIP_CALL ( SCIPaddCons(g,maximum_cost_cons) );
 
  //range >= desired
  SCIP_CONS* minimum_range_cons;
  double minimum_range_value=10.0;
  //SCIP_CALL ( SCIPcreateConsNonlinear(g,&minimum_range_cons, "Minimum_Range" ,exp_range,minimum_range_value, SCIPinfinity(g), CONS_DEFAULT));
  //SCIP_CALL ( SCIPaddCons(g,minimum_range_cons) );
    
  //speed>= desired
  SCIP_CONS* minimum_speed_cons;
  double minimum_speed_value=0.0;//it's actually 90, twr>1.0 and speed = 90*twr
  //SCIP_CALL ( SCIPcreateConsNonlinear(g,&minimum_speed_cons, "Minumum_Speed" ,exp_speed, minimum_speed_value, SCIPinfinity(g), CONS_DEFAULT));
  //SCIP_CALL ( SCIPaddCons(g,minimum_speed_cons) );

  //Weight: sum_weight< X
  SCIP_CONS* maximum_weight_cons;
  double maximum_weight_value=SCIPinfinity(g);
  SCIP_CALL ( SCIPcreateConsNonlinear(g,&maximum_weight_cons, "Maximum_Weight", sum_weight,0.0,maximum_weight_value, CONS_DEFAULT) );
  SCIP_CALL ( SCIPaddCons(g,maximum_weight_cons));
  

  SCIP_CONS* minimum_combat_time_cons;
  double combat_time_minimum=0.0;
  SCIP_CALL( SCIPcreateConsNonlinear(g,&minimum_combat_time_cons,"Minimum Combat Time", exp_combat_time, combat_time_minimum, SCIPinfinity(g), CONS_DEFAULT) );
  SCIP_CALL( SCIPaddCons(g,minimum_combat_time_cons));

  SCIP_CALL( SCIPprintOrigProblem(g, NULL, "cip", FALSE) ); 
  SCIP_CALL ( SCIPsolve(g) );
  if( SCIPgetNSols(g) > 0 )
  {
    SCIPinfoMessage(g, NULL, "\nSolution:\n");
    SCIP_CALL( SCIPprintSol(g, SCIPgetBestSol(g), NULL, FALSE) );
  }

  return SCIP_OKAY;

  return 0;
}

int execopt(int argc, char** argv){
  ModuleSet all;
  std::vector<size_t> counts;
  std::vector<module> available_mods(all_modules.size());
  for (auto m: all_modules){
    available_mods.push_back(m);
  }
  return solve(counts, available_mods );
}


int exectest(int argc, char** argv){
  SCIPLOCK lock;//RAII-ish
  SCIP_CALL( SCIPcreate(&g));
  SCIP_CALL( SCIPincludeDefaultPlugins(g));
  SCIP_CALL( SCIPcreateProbBasic(g, "Highfleet_Component_Selection"));
  SCIP_CALL( SCIPsetObjsense(g, SCIP_OBJSENSE_MINIMIZE));

  SCIP_CONS* c;
  SCIP_CONSHDLR* h;
  SCIP_CONSDATA* d;

  //create vars
  //pass those vars into expressions?
  //add expression to constraint
  //add constraint to problem
  SCIP_EXPR* prod;
  SCIP_EXPR* ex_vars[2];
  SCIP_VAR* vars[2];
  //create vars, expressions w/ thsoe vars, combine exprs, and set constraints
  SCIP_CALL ( SCIPcreateVarBasic(g,&vars[0], "a",1.0,SCIPinfinity(g), 2.0, SCIP_VARTYPE_INTEGER) ) ;
  SCIP_CALL ( SCIPcreateVarBasic(g,&vars[1], "b",1.0,SCIPinfinity(g), 2.0, SCIP_VARTYPE_INTEGER) );
  SCIP_CALL ( SCIPaddVar(g,vars[0]) );
  SCIP_CALL ( SCIPaddVar(g,vars[1]) );
  SCIP_CALL ( SCIPcreateExprVar(g,&ex_vars[0],vars[0],NULL,NULL) );
  SCIP_CALL ( SCIPcreateExprVar(g,&ex_vars[1],vars[1],NULL,NULL) );
  SCIP_CALL ( SCIPcreateExprProduct(g,&prod,2,ex_vars,1.0,NULL,NULL) );
  SCIP_CALL ( SCIPcreateConsNonlinear(g,&c, "Prod",prod, 0.0, 1.0, 
      1, 1, 1, 1, 1, 
      0, 0, 0, 0) );
  SCIP_CALL ( SCIPaddCons(g,c) );

  // Constraints https://www.scipopt.org/doc/html/group__CONSHDLRS.php
  // best example:
  // https://www.scipopt.org/doc/html/brachistochrone_8c_source.php#l00068

  ///
  SCIP_CALL( SCIPprintOrigProblem(g, NULL, "cip", FALSE) ); 
  SCIP_CALL ( SCIPsolve(g) );
  if( SCIPgetNSols(g) > 0 )
  {
    SCIPinfoMessage(g, NULL, "\nSolution:\n");
    SCIP_CALL( SCIPprintSol(g, SCIPgetBestSol(g), NULL, FALSE) );
  }

  return SCIP_OKAY;
}
int main(int argc, char** argv){
  //parse args ...
  
  return execopt(argc,argv)!=SCIP_OKAY ? 1:0;
}
