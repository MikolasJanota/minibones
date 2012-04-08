/*
 * File:  PicoWrap.hh
 * Author:  mikolas
 * Created on:  Sun, Apr 08, 2012 10:12:57 AM
 * Copyright (C) 2012, Mikolas Janota
 */
#ifndef PICOWRAP_HH_22116
#define PICOWRAP_HH_22116
#include "core/SolverTypes.h"
#include "picosat_wrapper_incr.hh"
using Minisat::lbool;
using Minisat::mkLit;
using Minisat::sign;
using Minisat::var;
using Minisat::vec;
using Minisat::Lit;
using Minisat::Var;

class PicoWrap {
public:
  vec<lbool>          model;
  PicoWrap();
  ~PicoWrap();
  Var                 newVar();
  bool                addClause(const vec<Lit>& ps);
  bool                solve();
  bool                solve(const vec<Lit>& assumptions);
private:
  IDManager           idm;
  PicosatWrapperIncr  picosat;
  BasicClauseSet      temporary_clauses;
  inline LINT         integer(Lit l);
};

inline LINT PicoWrap::integer(Lit l) { return  sign(l) ? -var(l) : var(l); }
#endif /* PICOWRAP_HH_22116 */
