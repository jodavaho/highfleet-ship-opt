#ifndef scip_help_hpp
#define scip_help_hpp

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

struct SCIPLOCK{
  SCIPLOCK(SCIP** g):g_(g){}
  ~SCIPLOCK(){
    if(g_){SCIPfree(g_);}
  }
  SCIP** g_;
};

struct EXPRDel{
  EXPRDel(SCIP*g, SCIP_EXPR**v):g_(g),v_(v)
  {};
  ~EXPRDel(){SCIPreleaseExpr(g_,v_);}
  SCIP* g_;
  SCIP_EXPR** v_;
};

struct VARDel{
  VARDel(SCIP*g, SCIP_VAR**v):g_(g),v_(v)
  {};
  ~VARDel(){SCIPreleaseVar(g_,v_);}
  SCIP* g_;
  SCIP_VAR** v_;
};

struct CONSDel{
  CONSDel(SCIP*g, SCIP_CONS**v):g_(g),v_(v)
  {};
  ~CONSDel(){SCIPreleaseCons(g_,v_);}
  SCIP* g_;
  SCIP_CONS** v_;
};

#endif
