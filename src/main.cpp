#include <fstream>
#include <iostream>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <vector>


SCIP* g = nullptr;
std::vector<SCIP_VAR*> Xi;
std::vector<SCIP_CONS*> cons_nl;
SCIP_SOL * solution = nullptr;
SCIP_CONSHDLR * conshdlr_nl;

struct SCIPLOCK{
  SCIPLOCK(){}
  ~SCIPLOCK(){
    for (auto ptr: cons_nl){
      SCIPreleaseCons(g,&ptr);      
    }
    for (auto ptr: Xi){
      SCIPreleaseVar(g,&ptr);
    }
    if(g){SCIPfree(&g);}
  }
};

int exectest(int argc, char** argv){
  //https://www.zib.de/hendel/talk/2019/monash_using_scip_to_solve/slides.pdf
  SCIPLOCK lock;//RAII
  SCIP_CALL( SCIPcreate(&g));
  //SCIP_CALL( SCIPincludeDefaultPlugins(g));
  SCIP_CALL( SCIPcreateProbBasic(g, "Highfleet_Component_Selection"));
  SCIP_CALL( SCIPsetObjsense(g, SCIP_OBJSENSE_MINIMIZE));
  SCIP_CALL( SCIPincludeConshdlrNonlinear(g) );

  SCIP_CONS* c;
  SCIP_CONSHDLR* h;
  SCIP_CONSDATA* d;

  //create vars
  //pass those vars into expressions?
  //add expression to constraint
  //add constraint to problem
  SCIP_EXPR* prod;
  SCIP_EXPR* ex_vars[10];
  SCIP_VAR* vars[10];
  //create vars, expressions w/ thsoe vars, combine exprs, and set constraints
  SCIPcreateVarBasic(g,&vars[0], "a",0,SCIPinfinity(g), 1.0, SCIP_VARTYPE_INTEGER);//etc
  SCIPcreateExprVar(g,&ex_vars[0],vars[0],nullptr,nullptr);//etc
  SCIPcreateExprProduct(g,&prod,10,ex_vars,1.0,NULL,NULL);
  SCIPcreateConsNonlinear(g,&c, "Ratio",prod, 0.0, 1.0, 
      1, 1, 1, 1, 1, 
      0, 0, 0, 0);

  // Constraints https://www.scipopt.org/doc/html/group__CONSHDLRS.php

  ///
  
  SCIP_CALL ( SCIPsolve(g) );

  return SCIP_OKAY;
}
int main(int argc, char** argv){
  //parse args ...

  return exectest(argc,argv)!=SCIP_OKAY ? 1:0;
}
