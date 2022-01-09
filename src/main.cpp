#include <fstream>
#include <iostream>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <scip/type_nlhdlr.h>
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
  SCIP_EXPR* ex;
  SCIP_VAR* v;
  SCIPcreateVarBasic(g,&v, "a",0,SCIPinfinity(g), 1.0, SCIP_VARTYPE_INTEGER);
  SCIPcreateExprProduct(g,&ex,2,
  SCIPcreateConsNonlinear(g,&c, "Ratio",ex, 0.0, 1.0, 
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
