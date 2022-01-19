#ifndef scip_help_hpp
#define scip_help_hpp

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

SCIP* g = NULL;
std::vector<SCIP_VAR*> vars;
std::vector<SCIP_EXPR*> ex_vars;

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

struct sCONS{
  sCONS(SCIP*g):g_(g){};
  ~sCONS(){SCIPreleaseCons(g_,&p_);}
  operator SCIP_CONS*(){ return p_;}
  operator SCIP_CONS**(){ return &p_;}
  SCIP_CONS* p_;
  SCIP*      g_;
};

struct sEXPR{
  sEXPR(SCIP*g):g_(g){};
  ~sEXPR(){SCIPreleaseExpr(g_,&p_);}
  operator SCIP_EXPR*(){ return p_;}
  operator SCIP_EXPR**(){ return &p_;}
  SCIP_EXPR* p_;
  SCIP*      g_;
};

#endif
