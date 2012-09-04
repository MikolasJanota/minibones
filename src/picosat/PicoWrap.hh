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
#include "solver_config.hh"
#include "solver_factory.hh"
using Minisat::lbool;
using Minisat::mkLit;
using Minisat::sign;
using Minisat::var;
using Minisat::vec;
using Minisat::Lit;
using Minisat::Var;

class PicoConfig : public SATSolverConfig {
public:
  PicoConfig()  : _solver("picosat")  {}
  bool   chk_sat_solver(const char* tsolver) { return !strcmp(_solver, tsolver); }
  bool   get_incr_mode() { return true; }
  int    get_verbosity()  { return 0; }
private:
  const char* _solver;
};

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
  SATSolverFactory    factory;
  PicoConfig          config;
  SATSolverWrapper&   picosat;
  BasicClauseSet      temporary_clauses;
  inline LINT         integer(Lit l);
};

inline LINT PicoWrap::integer(Lit l) { return  sign(l) ? -var(l) : var(l); }
#endif /* PICOWRAP_HH_22116 */
