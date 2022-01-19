#ifndef hf_problem_hpp
#define hf_problem_hpp

#include "scip_help.hpp"

struct Bounds{
  double cost[2]={0,1e7};
  double weight[2]={0,1e7};
  double speed[2]={0,1e7};
  double range[2]={0,1e15};
  double twr[2]={1.0,1e7};
  double combat_time[2]={0,1e7};
  double min_pwr_ratio=1.0;
  double min_crew_ratio=1.0;
};

int solve(
    std::vector<size_t> &out_counts,
    const std::vector<module> &mods,
    const Bounds bounds=Bounds(),
    const std::vector<module> required={} ,
    const Options opts={}
    )
{

  SCIPLOCK lock;//RAII-ish
  SCIP_CALL( SCIPcreate(&g));
  SCIP_CALL( SCIPincludeDefaultPlugins(g));
  SCIP_CALL( SCIPcreateProbBasic(g, "Highfleet_Component_Selection"));
  SCIP_CALL( SCIPsetObjsense(g, SCIP_OBJSENSE_MINIMIZE));
  SCIP_CALL( SCIPsetRealParam(g, "limits/gap", 0.01) );
  size_t N = mods.size();
  vars.reserve(N);
  ex_vars.reserve(N);

  //tally up the required modules first
  std::unordered_map<std::string,size_t> mod_minimums;
  std::unordered_map<std::string,size_t> mod_maximums;
  for (const auto &mod: mods){
    mod_minimums[mod.name]=0;
    mod_maximums[mod.name]=100;
  }
  for (const auto &req: required){
    mod_minimums[req.name]++;
  }
  mod_minimums["bridge"]=1;
  mod_maximums["bridge"]=1;
 
  //create optimization variables for n_<mod> for each mod
  for (size_t i=0;i<N;i++){
    auto m = mods[i];
    std::stringstream namss;
    namss<<"n_"<<m.name;

    std::string nam = namss.str();
    auto varp = &vars[i];
    //create var for count
    size_t min = mod_minimums[m.name];
    size_t max = mod_maximums[m.name];
    assert(min>=0);
    assert(max<=100);
    SCIP_CALL ( SCIPcreateVarBasic(g,varp, nam.c_str(), min, max, m.cost, SCIP_VARTYPE_INTEGER) ) ;
    SCIP_CALL ( SCIPaddVar(g,*varp) );
    auto ex_varp = &ex_vars[i];
    //create xprvar for count
    SCIP_CALL ( SCIPcreateExprVar(g,ex_varp,*varp,NULL,NULL) );
    //std::cout<<"Created: "<<nam<<std::endl;
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
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = std::fabs(mods[i].fuel_rate);
    }
    SCIP_EXPR * terms[1];
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_fuel_rate,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }


  //TODO, put this into module.hpp like fuel_cap vs fuel_rate?
  SCIP_EXPR *sum_power_production=nullptr;
  SCIP_EXPR *sum_power_consumption=nullptr;
  {
    std::vector<SCIP_Real> coefficients_producers(N);
    std::vector<SCIP_Real> coefficients_consumers(N);

    for (size_t i=0;i<N;i++){
      if (mods[i].energy > 0){
        coefficients_producers[i] = std::fabs(mods[i].energy);
      } else {
        coefficients_consumers[i] = std::fabs(mods[i].energy);
      }
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_power_production,N,ex_vars.data(),coefficients_producers.data(),0.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_power_consumption,N,ex_vars.data(),coefficients_consumers.data(),0.0,NULL,NULL));
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

  SCIP_EXPR *sum_crew_have=nullptr;
  SCIP_EXPR *sum_crew_need=nullptr;
  {
    std::vector<SCIP_Real> coefficients_crew_have(N);
    std::vector<SCIP_Real> coefficients_crew_need(N);
    for (size_t i=0;i<N;i++){
      double crewr = mods[i].crew;
      if (crewr>0){
        coefficients_crew_have[i] = std::fabs(crewr);
      } else {
        coefficients_crew_need[i] = std::fabs(crewr);
      }
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_crew_have,N,ex_vars.data(),coefficients_crew_have.data(),0.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_crew_need,N,ex_vars.data(),coefficients_crew_need.data(),0.0,NULL,NULL));
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
   * Set hull constraints
   */

  SCIP_CONS * large_hull_included;
  SCIP_CONS * small_hull_included;
  if (opts.include_hull){
    //sum(n_X) for [X=needs small] == sum(n_Y) [Y=small hull] We don't choose
    //hull parts, we tally the # of hull spaces required for each type (large,
    //small), and let the algorithm choose hull parts to fit the need. That way
    //if hull parts change in cost / size, we just recalculate. If we
    //custom-designed which hull parts went with which item, we'd be up a creek 
    SCIP_EXPR* large_needs, *small_needs;
    SCIP_EXPR* large_has, *small_has;
    SCIP_EXPR* large_diff, *small_diff;
    SCIP_EXPR* terms[2];
    SCIP_Real coefficients[2];

    std::vector<SCIP_Real> small_requires(N);
    std::vector<SCIP_Real> large_requires(N);
    std::vector<SCIP_Real> small_provides(N);
    std::vector<SCIP_Real> large_provides(N);

    for (size_t i=0;i<N;i++){
      switch (mods[i].mount){
        case module::SMALL:{ small_requires[i] = mods[i].sq; break; }
        case module::LARGE: { large_requires[i] = 1; }
        case module::EXTERIOR:{ break; }
        case module::HULL: { 
                             if (mods[i].sq==16){
                               large_provides[i]=1;
                             } else {
                               small_provides[i]=mods[i].sq;
                             }
                             break; }
      }
    }

    SCIP_CALL( SCIPcreateExprSum(g,&small_needs, N, ex_vars.data(), small_requires.data(), 0.0, NULL, NULL));
    SCIP_CALL( SCIPcreateExprSum(g,&large_needs, N, ex_vars.data(), large_requires.data(), 0.0, NULL, NULL));
    SCIP_CALL( SCIPcreateExprSum(g,&small_has, N, ex_vars.data(), small_provides.data(), 0.0, NULL, NULL));
    SCIP_CALL( SCIPcreateExprSum(g,&large_has, N, ex_vars.data(), large_provides.data(), 0.0, NULL, NULL));

    terms[0]=small_has;
    terms[1]=small_needs;
    coefficients[0]=1.0;
    coefficients[1]=-1.0;
    SCIP_CALL( SCIPcreateExprSum(g,&small_diff, 2, terms,coefficients,0.0,NULL,NULL));
    SCIP_CALL( SCIPcreateConsBasicNonlinear(g,&small_hull_included, "Small Hull Included",small_diff,0.0,0.0));
    terms[0]=large_has;
    terms[1]=large_needs;
    coefficients[0]=1.0;
    coefficients[1]=-1.0;
    SCIP_CALL( SCIPcreateExprSum(g,&large_diff, 2, terms,coefficients,0.0,NULL,NULL));
    SCIP_CALL( SCIPcreateConsBasicNonlinear(g,&large_hull_included, "Large Hull Included",large_diff,0.0,0.0));
    SCIP_CALL( SCIPaddCons(g, small_hull_included) );
    SCIP_CALL( SCIPaddCons(g, large_hull_included) );
    SCIPreleaseExpr(g,&small_needs);
    SCIPreleaseExpr(g,&small_has);
    SCIPreleaseExpr(g,&small_diff);
    SCIPreleaseExpr(g,&large_needs);
    SCIPreleaseExpr(g,&large_has);
    SCIPreleaseExpr(g,&large_diff);
  }



  /* 
   * Set TWR Constraints, and make them consistent with speed constraints
  */
  double twr_lb_val = std::max(bounds.speed[0]/89.9, bounds.twr[0]);//TODO: missing constant
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

  /*
   * Set power production / use constraints as
   * production/consumption > lower_bound
   */
  SCIP_CONS* power_lb_cons;
  {
    //P/C > LB --> P - LB.C > 0
    SCIP_EXPR* power_lhs_lb, *power_lhs_ub;
    
    SCIP_EXPR* terms[2];
    terms[0]=sum_power_production;
    terms[1]=sum_power_consumption;
    //
    SCIP_Real coefficients[2];
    //
    coefficients[0]=1.0;
    coefficients[1]= - bounds.min_pwr_ratio;
    SCIP_CALL ( SCIPcreateExprSum(g,&power_lhs_lb,2,terms,coefficients,0.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&power_lb_cons, "Power Ratio LB" ,power_lhs_lb, 0.0, SCIPinfinity(g)));
    SCIP_CALL ( SCIPaddCons(g,power_lb_cons));
  }

  //assuming correct units:
  //via /u/jodavaho & /u/d0d0b1rd
  //Range = sum_fuel_cap / ( sum_fuel_rate ) * speed
  //
  //sum_fuel_cap / (sum_fuel_rate) * speed < RUP //who needs an upper bound?
  //sum_fuel_cap / (sum_fuel_rate) * speed > RLO
  SCIP_CONS* range_lb_cons;
  {
    //OK.
    // speed (km/h)  times fuel_cap / fuel_burn (T / T/H = H) = km
    // spd . fuel_cap / fuel_rate > LB
    // 89.9 T/W . fuel_cap / fuel_rate > LB
    // 89.9 T . fuel_cap > LB . W . fuel_rate
    // 89.9 T . fc - LB . w . fr > 0
    SCIP_EXPR* range_lhs_lb, *range_lhs_ub;
    
    SCIP_EXPR* prod_terms[2];
    SCIP_EXPR* TxFC;
    prod_terms[0]=sum_thrust_mtf;
    prod_terms[1]=sum_fuel_cap;
    SCIP_CALL ( SCIPcreateExprProduct(g,&TxFC,2,prod_terms,89.9,NULL,NULL));
    //
    SCIP_EXPR* WxFR;
    prod_terms[0]=sum_weight;
    prod_terms[1]=sum_fuel_rate;
    SCIP_CALL ( SCIPcreateExprProduct(g,&WxFR,2,prod_terms,1.0,NULL,NULL));
    //
    SCIP_EXPR* sum_terms[2];
    sum_terms[0]=TxFC;
    sum_terms[1]=WxFR;
    SCIP_Real coefficients[2];
    coefficients[0]=1.0;
    coefficients[1]= - bounds.range[0];
    SCIP_CALL ( SCIPcreateExprSum(g,&range_lhs_lb,2,sum_terms,coefficients,0.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&range_lb_cons, "Range LB" ,range_lhs_lb, 0.0, SCIPinfinity(g)));
    SCIP_CALL ( SCIPaddCons(g,range_lb_cons));
  }


  //via /u/d0d0b1rd
  //Combat Time: sum(fuel_cap) / (sum(fuel_rate)*50) in seconds
  //TODO: sum_fuel_cap / fuel_rate < CUB*50
  //TODO: sum_fuel_cap - fuel_rate * CUB*50 < 0
  //TODO: - sum_fuel_cap + fuel_rate * CUB*50 > 0
  //
  //TODO: sum_fuel_cap / fuel_rate > CLB*50
  //TODO: sum_fuel_cap - fuel_rate * CLB*50 > 0

  /*
   * OK
   */
  SCIP_CONS* maximum_cost_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&maximum_cost_cons, "Maximum_cost" ,sum_cost, bounds.cost[0], bounds.cost[1]));
  SCIP_CALL ( SCIPaddCons(g,maximum_cost_cons) );

  /*
   * Ammo balanced
  */
  SCIP_CONS* ammo_balanced_cons;
  SCIP_CALL( SCIPcreateConsBasicNonlinear(g,&ammo_balanced_cons,"Ammo Balanced", sum_ammo, 0, SCIPinfinity(g)) );
  SCIP_CALL( SCIPaddCons(g,ammo_balanced_cons));

  /*
   * Maximum weight
  */
  SCIP_CONS* maximum_weight_cons;
  SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&maximum_weight_cons, "Maximum_Weight", sum_weight,bounds.weight[0], bounds.weight[1]) );
  SCIP_CALL ( SCIPaddCons(g,maximum_weight_cons));

  /*
   * Set power production / use constraints as
   * production/consumption > lower_bound
   */
  SCIP_CONS* crew_ratio;
  {
    //Crew/Req > LB --> Crew - LB x Req > 0
    SCIP_EXPR* crew_lhs_lb;
    
    SCIP_EXPR* terms[2];
    terms[0]=sum_crew_have;
    terms[1]=sum_crew_need;
    //
    SCIP_Real coefficients[2];
    //
    coefficients[0]=1.0;
    coefficients[1]= - bounds.min_crew_ratio;
    SCIP_CALL ( SCIPcreateExprSum(g,&crew_lhs_lb,2,terms,coefficients,0.0,NULL,NULL));
    SCIP_CALL ( SCIPcreateConsBasicNonlinear(g,&crew_ratio, "Crew Ratio" ,crew_lhs_lb, 0.0, SCIPinfinity(g)));
    SCIP_CALL ( SCIPaddCons(g,crew_ratio));
  }
 
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

#endif
