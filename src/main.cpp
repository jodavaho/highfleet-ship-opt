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
    for (auto &ptr: ex_vars){
      SCIPreleaseExpr(g,&ptr);
    }
    for (auto &ptr: vars){
      SCIPreleaseVar(g,&ptr);
    }
    if(g){SCIPfree(&g);}
  }
};

struct Bounds{
  double cost[2]={0,1e7};
  double weight[2]={0,1e7};
  double speed[2]={0,1e7};
  double range[2]={0,1e15};
  double twr[2]={1.0,1e7};
  double combat_time[2]={0,1e7};
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
  SCIP_CALL( SCIPsetRealParam(g, "limits/gap", 0.05) );
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
    if (min>0){
       std::cout<<"req: "<<min<<"<n_"<<m.name<<"<"<<SCIPinfinity(g)<<std::endl;
    }
    SCIP_CALL ( SCIPcreateVarBasic(g,varp, nam.c_str(), min, SCIPinfinity(g), m.cost, SCIP_VARTYPE_INTEGER) ) ;
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
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_fuel_cap,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_fuel_rate=nullptr;
  SCIP_EXPR *sum_fuel_rate_3p6=nullptr;
  SCIP_EXPR *sum_fuel_rate_50=nullptr;
  SCIP_EXPR *inv_sum_fuel_rate=nullptr;
  SCIP_EXPR *inv_sum_fuel_rate_3p6=nullptr;
  SCIP_EXPR *inv_sum_fuel_rate_50=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = std::fabs(mods[i].fuel_rate);
    }
    SCIP_EXPR * terms[1];
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_fuel_rate,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
    terms[0]=sum_fuel_rate;
    //later need 1/(3.6 x)
    SCIP_CALL ( SCIPcreateExprProduct(g,&sum_fuel_rate_3p6,1,terms,3.6,NULL,NULL));
    SCIP_CALL ( SCIPcreateExprSignpower(g,&inv_sum_fuel_rate_3p6,sum_fuel_rate_3p6,-1.0,NULL,NULL));
    //later need 1/(50 x)
    SCIP_CALL ( SCIPcreateExprProduct(g,&sum_fuel_rate_50,1,terms,50.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateExprSignpower(g,&inv_sum_fuel_rate_50,sum_fuel_rate_50,-1.0,NULL,NULL));
    //just in case, 1/x
    SCIP_CALL ( SCIPcreateExprSignpower(g,&inv_sum_fuel_rate,sum_fuel_rate,-1.0,NULL,NULL));
  }

  SCIP_EXPR *sum_thrust_mn=nullptr;
  SCIP_EXPR *sum_thrust_mtf=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = std::fabs(mods[i].thrust);
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_thrust_mn,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
    SCIP_EXPR* terms[1];
    terms[0]=sum_thrust_mn;
    SCIP_CALL ( SCIPcreateExprProduct(g,&sum_thrust_mtf,1,terms,101.97,NULL,NULL));
  }
   
  SCIP_EXPR *sum_cost=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].cost;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_cost,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_ammo=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].ammo;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_ammo,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }


  SCIP_EXPR *sum_weight=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].weight;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_weight,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_crew=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].crew;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_crew,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }
  
  SCIP_EXPR *sum_energy=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].energy;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_energy,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_energy_production=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      if (mods[i].energy>0){
        vals[i]=mods[i].energy;
      }
      vals[i] = 0;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_energy_production,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_energy_consumption=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      if (mods[i].energy<0){
        vals[i]=mods[i].energy;
      }
      vals[i] = 0;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_energy_consumption,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_firepower=nullptr;
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].firepower;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_firepower,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  //////////////////// Design constraints
  //

  /* 
   * Set TWR Constraints, and make them consistent with speed constraints
  */
  double twr_lb_val = std::min(bounds.speed[0]/89.9, bounds.twr[0]);//TODO: missing constant
  double twr_ub_val = std::min(bounds.speed[1]/89.9, bounds.twr[1]);
  SCIP_CONS* twr_lb_cons;
  SCIP_CONS* twr_ub_cons;
  {
    //T/W > LB --> T - LB.W > 0
    //T/W < UB --> T - UB.W < 0 --> UB.W-T > 0
    SCIP_EXPR* twr_lhs_lb, *twr_lhs_ub;
    
    SCIP_EXPR* terms[2];
    terms[0]=sum_thrust_mtf;
    terms[1]=sum_weight;
    //
    SCIP_Real coefficients[2];
    //
    coefficients[0]=1.0;
    coefficients[1]=- twr_lb_val;
    SCIP_CALL ( SCIPcreateExprSum(g,&twr_lhs_lb,2,terms,coefficients,0.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&twr_lb_cons, "TWR LB" ,twr_lhs_lb, 0.0, SCIPinfinity(g)));
    SCIP_CALL ( SCIPaddCons(g,twr_lb_cons));
    // opposite sign (>0)
    coefficients[0]= twr_ub_val;
    coefficients[1]=-1.0;
    SCIP_CALL ( SCIPcreateExprSum(g,&twr_lhs_ub,2,terms,coefficients,0.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&twr_ub_cons, "TWR UB" ,twr_lhs_ub, 0.0, SCIPinfinity(g)));
    SCIP_CALL ( SCIPaddCons(g,twr_ub_cons));
    //
  }

  //assuming correct units:
  //via /u/jodavaho & /u/d0d0b1rd
  //Range = sum_fuel_cap / ( sum_fuel_rate ) * speed
  //sum_fuel_cap / (sum_fuel_rate) * speed < RUP
  //TODO: sum_fuel_cap * speed - RUP * sum_fuel_rate < 0
  //sum_fuel_cap / (sum_fuel_rate) * speed > RLO
  //TODO: sum_fuel_cap * speed - RLO * sum_fuel_rate > 0
  {
    //SCIP_EXPR* terms[3]={sum_fuel_cap,inv_sum_fuel_rate,exp_speed};
    //SCIP_CALL ( SCIPcreateExprProduct(g,&exp_range,3,terms,1.0,NULL,NULL));
  }

  //via /u/d0d0b1rd
  //Combat Time: sum(fuel_cap) / (sum(fuel_rate)*50) in seconds
  //TODO: sum_fuel_cap / fuel_rate < CUB*50
  //TODO: sum_fuel_cap - fuel_rate * CUB*50 < 0
  //TODO: sum_fuel_cap / fuel_rate > CLB*50
  //TODO: sum_fuel_cap - fuel_rate * CLB*50 > 0

  /*
   * OK
   */
  SCIP_CONS* maximum_cost_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&maximum_cost_cons, "Maximum_cost" ,sum_cost, bounds.cost[0], bounds.cost[1]));
  SCIP_CALL ( SCIPaddCons(g,maximum_cost_cons) );

  /*
   * OK
  */
  SCIP_CONS* ammo_balanced_cons;
  SCIP_CALL( SCIPcreateConsBasicNonlinear(g,&ammo_balanced_cons,"Ammo Balanced", sum_ammo, 0, 0) );
  SCIP_CALL( SCIPaddCons(g,ammo_balanced_cons));

  /*
   * RLB < speed * fuel_cap / fuel_rate 
   * TODO: 0 < speed * fuel_cap - RLB * fuel_rate
   * TODO: 0 > speed * fuel_cap - RUB * fuel_rate
  SCIP_CONS* minimum_range_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&minimum_range_cons, "Minimum_Range" ,exp_range,bounds.range[0],bounds.range[1]));
  SCIP_CALL ( SCIPaddCons(g,minimum_range_cons) );
  */
  //

  /*
   * OK
  */
  SCIP_CONS* maximum_weight_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&maximum_weight_cons, "Maximum_Weight", sum_weight,bounds.weight[0], bounds.weight[1]) );
  SCIP_CALL ( SCIPaddCons(g,maximum_weight_cons));
 
  /*
   * fuel_cap / (fuel_rate * 50 ) < CTUB
   * TODO: fuel_cap - CTUB * (fuel_rate * 50 ) < 0
   * fuel_cap / (fuel_rate * 50 ) > CTLB
   * TODO: fuel_cap - CTLB * (fuel_rate * 50 ) > 0
  */

  /*
   * energy_rate / energy_need > ELB
   * TODO: energy_rate - ELB * energy_need > 0
   * energy_rate / energy_need < EUB
   * TODO: energy_rate - EUB * energy_need < 0
   * for ELB, EUB \n [0,1]
   */
  //todo todo todo 

  SCIP_CALL( SCIPprintOrigProblem(g, NULL, "cip", FALSE) ); 
  //fucking compiled without support:
  //SCIP_CALL ( SCIPsolveParallel(g) );
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
    //std::cout<<"Adding: "<<m.name<<std::endl;
    std::cout<<"Adding: "<<m<<std::endl;
    available_mods.push_back(m);
  }
  Bounds b;
  b.twr[1]=5;
  b.twr[0]=1;
  std::vector<module> req;
  req.push_back( *by_name("RD_51") );
  req.push_back( *by_name("d_80") );
  req.push_back( *by_name("d_80") );
  return solve(counts, available_mods, b, req );
}

int main(int argc, char** argv){
 
  return execopt(argc,argv)!=SCIP_OKAY ? 1:0;
}
