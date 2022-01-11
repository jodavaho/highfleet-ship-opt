#include <fstream>
#include <iostream>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <vector>
#include <string>


SCIP* g = NULL;
std::vector<SCIP_VAR*> vars;
std::vector<SCIP_EXPR*> ex_vars;
std::vector<SCIP_EXPR*> sum_res;
std::vector<SCIP_EXPR*> exprs;
std::vector<SCIP_CONS*> cons;
SCIP_SOL * solution = NULL;
SCIP_CONSHDLR * conshdlr_nl;

struct SCIPLOCK{
  SCIPLOCK(){}
  ~SCIPLOCK(){
    for (auto ptr: cons){
      SCIPreleaseCons(g,&ptr);      
    }
    for (auto ptr: sum_res){
      SCIPreleaseExpr(g,&ptr);
    }
    for (auto ptr: exprs){
      SCIPreleaseExpr(g,&ptr);
    }
    for (auto ptr: ex_vars){
      SCIPreleaseExpr(g,&ptr);
    }
    for (auto ptr: vars){
      SCIPreleaseVar(g,&ptr);
    }
    if(g){SCIPfree(&g);}
  }
};

struct module{
  std::string name;
  static const size_t n_res=8;
  double cost;
  double weight;
  double energy;
  double crew;
  double thrust;
  double fuel_cap;
  double fuel_rate;
  double firepower;
  std::pair<double,double> sz;
};

extern module Bridge;

int solve(
    std::vector<module> mods,
    std::vector<module> required = {Bridge}
    ){
  SCIPLOCK lock;//RAII-ish
  SCIP_CALL( SCIPcreate(&g));
  SCIP_CALL( SCIPincludeDefaultPlugins(g));
  SCIP_CALL( SCIPcreateProbBasic(g, "Highfleet_Component_Selection"));
  SCIP_CALL( SCIPsetObjsense(g, SCIP_OBJSENSE_MINIMIZE));
  size_t N = mods.size();
  vars.reserve(N);
  ex_vars.reserve(N);
  sum_res.reserve(module::n_res);//sum of all resources
  exprs.reserve(module::n_res); 
 
  //create optimization variables for n_<mod> for each mod
  for (size_t i=0;i<N;i++){
    auto m = mods[i];
    const char* nam = ("n_"+m.name).c_str();
    auto varp = &vars[i];
    //create var for count
    SCIP_CALL ( SCIPcreateVarBasic(g,varp, nam,0.0,SCIPinfinity(g), 0.0, SCIP_VARTYPE_INTEGER) ) ;
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
  {
    std::vector<double> vals(N);
    for (size_t i=0;i<N;i++){
      vals[i] = mods[i].fuel_rate;
    }
    SCIP_CALL ( SCIPcreateExprSum(g,&sum_fuel_rate,N,ex_vars.data(),vals.data(),1.0,NULL,NULL));
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

  return 0;
}

int execopt(int argc, char** argv){

  return 0;
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

  return exectest(argc,argv)!=SCIP_OKAY ? 1:0;
}
