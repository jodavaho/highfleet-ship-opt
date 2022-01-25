#include "hf-problem.hpp"

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#define SCIP_CHK(x)   do                                                                                     \
                       {                                                                                      \
                          SCIP_RETCODE _restat_;                                                              \
                          if( (_restat_ = (x)) != SCIP_OKAY )                                                 \
                          {                                                                                   \
                             SCIPerrorMessage("Error <%d> in function call\n", _restat_);                     \
                             return ERR_INTERNAL;                                                                 \
                           }                                                                                  \
                       }                                                                                      \
                       while( FALSE )


SOLVECODE solve(
    std::vector<size_t> &out_counts,
    const std::vector<module> &mods,
    const Bounds bounds,
    const std::vector<module> required,
    const SolveOptions opts
    )

{

  SCIP* g; 
  SCIP_CHK( SCIPcreate(&g) );
  SCIP_CHK( SCIPcreateProbBasic(g, "Highfleet_Component_Selection"));
  SCIP_CHK( SCIPincludeDefaultPlugins(g));
  SCIP_CHK( SCIPsetObjsense(g, SCIP_OBJSENSE_MINIMIZE));
  SCIP_CHK( SCIPsetRealParam(g, "limits/gap", 0.01) );
  size_t N = mods.size();
  out_counts.resize(N);
  std::vector<SCIP_VAR*> vars(N);
  std::vector<SCIP_EXPR*> ex_vars(N);
  std::vector<SCIP_EXPR**> other_expr;
  std::vector<SCIP_CONS**> other_cons;

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
    SCIP_CHK ( SCIPcreateVarBasic(g,varp, nam.c_str(), min, max, m.cost, SCIP_VARTYPE_INTEGER) ) ;
    SCIP_CHK ( SCIPaddVar(g,*varp) );
    auto ex_varp = &ex_vars[i];
    //create xprvar for count
    SCIP_CHK ( SCIPcreateExprVar(g,ex_varp,*varp,NULL,NULL) );
  }

  //create the sum_of(resource) for each resource
  SCIP_EXPR *sum_fuel_cap=nullptr;
  other_expr.push_back(&sum_fuel_cap);
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].fuel_cap;
    }
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_fuel_cap,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_fuel_rate=nullptr;
  other_expr.push_back(&sum_fuel_rate);
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = std::fabs(mods[i].fuel_rate);
    }
    SCIP_EXPR * terms[1];
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_fuel_rate,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }


  //TODO, put this into module.hpp like fuel_cap vs fuel_rate?
  SCIP_EXPR *sum_power_production=nullptr;
  other_expr.push_back(&sum_power_production);
  SCIP_EXPR *sum_power_consumption=nullptr;
  other_expr.push_back(&sum_power_consumption);
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
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_power_production,N,ex_vars.data(),coefficients_producers.data(),0.0,NULL,NULL));
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_power_consumption,N,ex_vars.data(),coefficients_consumers.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_thrust_mn=nullptr;
  SCIP_EXPR *sum_thrust_mtf=nullptr;
  other_expr.push_back(&sum_thrust_mn);
  other_expr.push_back(&sum_thrust_mtf);
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = std::fabs(mods[i].thrust);
    }
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_thrust_mn,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
    SCIP_EXPR* terms[1];
    terms[0]=sum_thrust_mn;
    SCIP_CHK ( SCIPcreateExprProduct(g,&sum_thrust_mtf,1,terms,101.97,NULL,NULL));
  }
   
  SCIP_EXPR *sum_cost=nullptr;
  other_expr.push_back(&sum_cost);
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].cost;
    }
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_cost,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_ammo=nullptr;
  other_expr.push_back(&sum_ammo);
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].ammo;
    }
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_ammo,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_weight=nullptr;
  other_expr.push_back(&sum_weight);
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].weight;
    }
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_weight,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  SCIP_EXPR *sum_crew_have=nullptr;
  SCIP_EXPR *sum_crew_need=nullptr;
  other_expr.push_back(&sum_crew_have);
  other_expr.push_back(&sum_crew_need);
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
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_crew_have,N,ex_vars.data(),coefficients_crew_have.data(),0.0,NULL,NULL));
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_crew_need,N,ex_vars.data(),coefficients_crew_need.data(),0.0,NULL,NULL));
  }
  
  SCIP_EXPR *sum_firepower=nullptr;
  other_expr.push_back(&sum_firepower);
  {
    std::vector<SCIP_Real> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].firepower;
    }
    SCIP_CHK ( SCIPcreateExprSum(g,&sum_firepower,N,ex_vars.data(),vals.data(),0.0,NULL,NULL));
  }

  //////////////////// Design constraints
  //

  /*
   * Set hull constraints
   */

  SCIP_CONS * large_hull_included;
  SCIP_CONS * small_hull_included;
  other_cons.push_back(&large_hull_included);
  other_cons.push_back(&small_hull_included);
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
        case module::RUNWAY:{ break;}//this is TODO
        case module::HULL: { 
                             if (mods[i].sq==16){
                               large_provides[i]=1;
                             } else {
                               small_provides[i]=mods[i].sq;
                             }
                             break; }
      }
    }

    SCIP_CHK( SCIPcreateExprSum(g,&small_needs, N, ex_vars.data(), small_requires.data(), 0.0, NULL, NULL));
    SCIP_CHK( SCIPcreateExprSum(g,&large_needs, N, ex_vars.data(), large_requires.data(), 0.0, NULL, NULL));
    SCIP_CHK( SCIPcreateExprSum(g,&small_has, N, ex_vars.data(), small_provides.data(), 0.0, NULL, NULL));
    SCIP_CHK( SCIPcreateExprSum(g,&large_has, N, ex_vars.data(), large_provides.data(), 0.0, NULL, NULL));

    terms[0]=small_has;
    terms[1]=small_needs;
    coefficients[0]=1.0;
    coefficients[1]=-1.0;
    SCIP_CHK( SCIPcreateExprSum(g,&small_diff, 2, terms,coefficients,0.0,NULL,NULL));
    SCIP_CHK( SCIPcreateConsBasicNonlinear(g,&small_hull_included, "Small Hull Included",small_diff,0.0,0.0));
    terms[0]=large_has;
    terms[1]=large_needs;
    coefficients[0]=1.0;
    coefficients[1]=-1.0;
    SCIP_CHK( SCIPcreateExprSum(g,&large_diff, 2, terms,coefficients,0.0,NULL,NULL));
    SCIP_CHK( SCIPcreateConsBasicNonlinear(g,&large_hull_included, "Large Hull Included",large_diff,0.0,0.0));
    SCIP_CHK( SCIPaddCons(g, small_hull_included) );
    SCIP_CHK( SCIPaddCons(g, large_hull_included) );


    SCIPreleaseExpr(g,&small_needs);
    SCIPreleaseExpr(g,&small_has);
    SCIPreleaseExpr(g,&large_needs);
    SCIPreleaseExpr(g,&large_has);
    SCIPreleaseExpr(g,&small_diff);
    SCIPreleaseExpr(g,&large_diff);
  }



  /* 
   * Set TWR Constraints, and make them consistent with speed constraints
  */
  double twr_lb_val = std::max(bounds.spd_min/89.9, bounds.twr_min);//TODO: missing constant
  double twr_ub_val = std::min(bounds.spd_max/89.9, bounds.twr_max);
  SCIP_CONS* twr_lb_cons;
  SCIP_CONS* twr_ub_cons;
  other_cons.push_back(&twr_lb_cons);
  other_cons.push_back(&twr_ub_cons);
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
    SCIP_CHK ( SCIPcreateExprSum(g,&twr_lhs_lb,2,terms,coefficients,0.0,NULL,NULL));
    SCIP_CHK ( SCIPcreateConsBasicNonlinear(g,&twr_lb_cons, "TWR LB" ,twr_lhs_lb, 0.0, SCIPinfinity(g)));
    SCIP_CHK ( SCIPaddCons(g,twr_lb_cons));
    // opposite sign (>0)
    coefficients[0]=-1.0;
    coefficients[1]= twr_ub_val;
    SCIP_CHK ( SCIPcreateExprSum(g,&twr_lhs_ub,2,terms,coefficients,0.0,NULL,NULL));
    SCIP_CHK ( SCIPcreateConsBasicNonlinear(g,&twr_ub_cons, "TWR UB" ,twr_lhs_ub, 0.0, SCIPinfinity(g)));
    SCIP_CHK ( SCIPaddCons(g,twr_ub_cons));

    SCIPreleaseExpr(g, &twr_lhs_lb);
    SCIPreleaseExpr(g, &twr_lhs_ub);
    //
  }

  /*
   * Set power production / use constraints as
   * production/consumption > lower_bound
   */
  {
    SCIP_CONS* power_lb_cons;
    //P/C > LB --> P - LB.C > 0
    SCIP_EXPR* power_lhs_lb;
    
    SCIP_EXPR* terms[2];
    terms[0]=sum_power_production;
    terms[1]=sum_power_consumption;
    //
    SCIP_Real coefficients[2];
    //
    coefficients[0]=1.0;
    coefficients[1]= - bounds.pwr_ratio_min;
    SCIP_CHK ( SCIPcreateExprSum(g,&power_lhs_lb,2,terms,coefficients,0.0,NULL,NULL));
    SCIP_CHK ( SCIPcreateConsBasicNonlinear(g,&power_lb_cons, "Power Ratio LB" ,power_lhs_lb, 0.0, SCIPinfinity(g)));
    SCIP_CHK ( SCIPaddCons(g,power_lb_cons));

    SCIPreleaseCons(g,&power_lb_cons);
    SCIPreleaseExpr(g,&power_lhs_lb);
  }

  //assuming correct units:
  //via /u/jodavaho & /u/d0d0b1rd
  //Range = sum_fuel_cap / ( sum_fuel_rate ) * speed
  //
  //sum_fuel_cap / (sum_fuel_rate) * speed < RUP //who needs an upper bound?
  //sum_fuel_cap / (sum_fuel_rate) * speed > RLO
  {
    SCIP_CONS* range_lb_cons;
    // speed (km/h)  times fuel_cap / fuel_burn (T / T/H = H) = km
    // spd . fuel_cap / fuel_rate > LB
    // 89.9 T/W . fuel_cap / fuel_rate > LB
    // 89.9 T . fuel_cap > LB . W . fuel_rate
    // 89.9 T . fc - LB . w . fr > 0
    SCIP_EXPR* range_lhs_lb;
    
    SCIP_EXPR* prod_terms[2];
    SCIP_EXPR* TxFC;
    prod_terms[0]=sum_thrust_mtf;
    prod_terms[1]=sum_fuel_cap;
    SCIP_CHK ( SCIPcreateExprProduct(g,&TxFC,2,prod_terms,89.9,NULL,NULL));
    //
    SCIP_EXPR* WxFR;
    prod_terms[0]=sum_weight;
    prod_terms[1]=sum_fuel_rate;
    SCIP_CHK ( SCIPcreateExprProduct(g,&WxFR,2,prod_terms,1.0,NULL,NULL));
    //
    SCIP_EXPR* sum_terms[2];
    sum_terms[0]=TxFC;
    sum_terms[1]=WxFR;
    SCIP_Real coefficients[2];
    coefficients[0]=1.0;
    coefficients[1]= - bounds.range_min;

    SCIP_CHK ( SCIPcreateExprSum(g,&range_lhs_lb,2,sum_terms,coefficients,0.0,NULL,NULL));
    SCIP_CHK ( SCIPcreateConsBasicNonlinear(g,&range_lb_cons, "Range LB" ,range_lhs_lb, 0.0, SCIPinfinity(g)));
    SCIP_CHK ( SCIPaddCons(g,range_lb_cons));
    SCIP_CHK ( SCIPreleaseCons(g,&range_lb_cons));
    SCIP_CHK ( SCIPreleaseExpr(g,&range_lhs_lb));
    SCIP_CHK ( SCIPreleaseExpr(g,&WxFR));
    SCIP_CHK ( SCIPreleaseExpr(g,&TxFC));
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
  SCIP_CHK ( SCIPcreateConsBasicNonlinear(g,&maximum_cost_cons, "Maximum_cost" ,sum_cost, bounds.cost_min, bounds.cost_max));
  SCIP_CHK ( SCIPaddCons(g,maximum_cost_cons) );
  SCIP_CHK ( SCIPreleaseCons(g,&maximum_cost_cons) );

  /*
   * Ammo balanced
  */
  SCIP_CONS* ammo_balanced_cons;
  SCIP_CHK( SCIPcreateConsBasicNonlinear(g,&ammo_balanced_cons,"Ammo Balanced", sum_ammo, 0, SCIPinfinity(g)) );
  SCIP_CHK( SCIPaddCons(g,ammo_balanced_cons));
  SCIP_CHK( SCIPreleaseCons(g, &ammo_balanced_cons ));

  /*
   * Maximum weight
  */
  SCIP_CONS* maximum_weight_cons;
  SCIP_CHK ( SCIPcreateConsBasicNonlinear(g,&maximum_weight_cons, "Maximum_Weight", sum_weight,bounds.wt_min, bounds.wt_max) );
  SCIP_CHK ( SCIPaddCons(g,maximum_weight_cons));
  SCIP_CHK ( SCIPreleaseCons(g, &maximum_weight_cons));

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
    coefficients[1]= - bounds.crew_ratio_min;
    SCIP_CHK ( SCIPcreateExprSum(g,&crew_lhs_lb,2,terms,coefficients,0.0,NULL,NULL));
    SCIP_CHK ( SCIPcreateConsBasicNonlinear(g,&crew_ratio, "Crew Ratio" ,crew_lhs_lb, 0.0, SCIPinfinity(g)));
    SCIP_CHK ( SCIPaddCons(g,crew_ratio));
  }
 
  SCIP_CHK( SCIPprintOrigProblem(g, NULL, "cip", FALSE) ); 
  //fucking compiled without support:
  //SCIP_CHK ( SCIPsolveParallel(g) );
  SCIP_CHK ( SCIPsolve(g) );
  if( SCIPgetNSols(g) > 0 )
  {
    SCIPinfoMessage(g, NULL, "\nSolution:\n");
    auto sol = SCIPgetBestSol(g);
    SCIP_CHK( SCIPprintSol(g, sol, NULL, FALSE) );
    for (int i=0;i<N;i++){
      out_counts[i]=(size_t) std::round(SCIPgetSolVal(g,sol,vars[i]));
    }
  } else {
    return ERR_INFEASIBLE;
  }

  for (auto conspp: other_cons){
    SCIPreleaseCons(g,conspp);
  }
  for (auto exprpp: other_expr){
    SCIPreleaseExpr(g,exprpp);
  }
  for (int i=0;i<N;i++){
    SCIPreleaseExpr(g,&ex_vars[i]);
    SCIPreleaseVar(g,&vars[i]);
  }
  SCIPfree(&g);

  return OK;
}
