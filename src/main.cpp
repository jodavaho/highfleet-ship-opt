#include <fstream>
#include <iostream>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <vector>


SCIP* g = NULL;
std::vector<SCIP_VAR*> Xi;
std::vector<SCIP_CONS*> cons_nl;
SCIP_SOL * solution = NULL;
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
