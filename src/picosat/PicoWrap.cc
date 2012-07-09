/*
 * File:  PicoWrap.cc
 * Author:  mikolas
 * Created on:  Sun, Apr 08, 2012 10:18:09 AM
 * Copyright (C) 2012, Mikolas Janota
 */
#include "PicoWrap.hh"
#include "minisat_aux.hh"

PicoWrap::PicoWrap()
  : factory(idm) 
  , picosat(factory.instance(config))
{
  picosat.init_all(); 
}

PicoWrap::~PicoWrap(){
  picosat.reset_all();
}

Var PicoWrap::newVar() { return (Var) idm.new_id(); }

bool PicoWrap::addClause(const vec<Lit>& ps) {
  vector<LINT> temporary(ps.size());  
  for (int i=0; i<ps.size(); ++i) temporary[i] = integer(ps[i]); 
  BasicClause* const cl = temporary_clauses.create_clause(temporary);
  assert(cl);
  picosat.add_final_clause(cl);
  return cl!=NULL;
}

bool PicoWrap::solve() {
  const vec<Lit> assumptions;
  return solve(assumptions);
} 

bool  PicoWrap::solve(const vec<Lit>& assumptions) {
  vector<LINT> temporary(assumptions.size());
  for (int i=0; i<assumptions.size(); ++i) temporary[(size_t)i] = integer(assumptions[i]); 

  picosat.init_run();
  const auto r = ((PicosatWrapperIncr&)picosat).solve(temporary);
  bool return_value;
  switch (r) {
  case SAT_True: return_value = true; break;
  case SAT_False: return_value = false; break;
  case SAT_NoRes:
  case SAT_Abort:
    assert (false);
    return false;
    //TODO
  default: 
    assert (false);
    return false;
  }
  if (return_value)  {
    IntVector temporary_model;
    picosat.get_model(temporary_model);
    model.growTo(temporary_model.size());
    for (size_t i=0; i< temporary_model.size (); ++i) {
      const auto val = temporary_model[i];
      model[(int)i] = val ? (val > 0 ? l_True : l_False) : l_Undef; 
    }
  }

  picosat.reset_run();
  return return_value;
}

