/*
 * File:  UpperBound.cc
 * Author:  mikolas
 * Created on:  Tue, Feb 21, 2012 3:45:21 PM
 * Copyright (C) 2012, Mikolas Janota
 */
#include "UpperBound.hh"
using namespace minibones; 

#define UPPERBOUND_DBG(t)

/*------------------------- initialization -----------------------------------*/
UpperBound::UpperBound(ToolConfig& _tool_configuration, ostream& _output,  Var _max_id, const CNF& _clauses)
  : tool_configuration(_tool_configuration)
  , output(_output)
  , max_id(_max_id)
  , clauses(_clauses)
  , solver_calls(0)
  , lifter(clauses)
  , rotatable_computer(clauses)
{
  assert(variable_range.first <= 1);
}

UpperBound::~UpperBound() {}

bool UpperBound::initialize() {
  //initialize the solver
  for (Var i=0; i<=max_id; ++i) solver.newVar();
  vec<Lit> ls;
  FOR_EACH(ci, clauses) {
    ls.clear ();
    FOR_EACH(li, *ci) {
      const Lit l = *li;
      assert (var(l) <= max_id);
      ls.push(l);
    }
    solver.addClause(ls);
  }

  // get the first model
  const bool result = solver.solve();
  if (!result) {// the whole instance is not satisfiable
    return false;
  }

  // TODO apply initial pruning

  const vec<lbool>& model = solver.model;
  UPPERBOUND_DBG( print_model(cerr << "initial model: ", model) << endl; );

  for (Var variable = 1; variable <= max_id; ++variable) {
    if (max_id >= model.size()) continue;
    const lbool value = model[variable];
    if (value != l_Undef) {
      const Lit l = mkLit(variable, value==l_False);
      might_be.add(l);
    }
  }

  UPPERBOUND_DBG( might_be.print(cerr << "might be:")<< endl; );

  process_model(solver.model);
  return true;
}

/*----------------------  run of the worker ----------------------------------*/

void UpperBound::run() {
  vec<Lit> literals;
  vec<Lit> assumptions(1);
  Var relaxation_variable = 0;
  while (true) {
    UPPERBOUND_DBG( might_be.print(cerr << "might be:")<< endl; );
    // prepare clause that states that we are looking for something that improves the current upper bound of the backbone
    literals.clear();
    if (relaxation_variable) solver.addClause(mkLit(relaxation_variable)); // relax the last formula (forever)
    relaxation_variable = solver.newVar();      // generate new relaxation variable
    literals.push(mkLit(relaxation_variable));  // enabled the new clause to be relaxed
    for (Var v=1; v<=max_id; ++v) {
      const Lit pos_lit = mkLit(v);
      const Lit neg_lit = ~mkLit(v);
      const bool might_be_pos = might_be.get(pos_lit);
      const bool might_be_neg = might_be.get(neg_lit);
      assert (!might_be_pos || might_be_neg);
      if (might_be_pos) literals.push(neg_lit);
      if (might_be_neg) literals.push(pos_lit);
    }
    solver.addClause(literals);
    UPPERBOUND_DBG( cerr << "ref: " << literals << endl; );
    assumptions[0] = ~mkLit(relaxation_variable); // disallow relaxing current refinement clause
    const bool is_sat = solver.solve(assumptions);
    if (!is_sat) return;
    const vec<lbool>& model = solver.model;
    process_model(model);
  }
}

void UpperBound::process_model(const vec<lbool>& model) {
  if (tool_configuration.get_scdc_pruning()) {
    vec<lbool> rmodel;
    vec<Lit> assumptions;
    lifter.reduce(assumptions, model, rmodel);
    assert(lifter.is_model(rmodel));
    process_pruned_model(rmodel);
  } else if (tool_configuration.get_rotatable_pruning ()) {
    VarSet pruned;
    rotatable_computer.get_rotatables(solver.model, pruned);
    FOR_EACH (vi, pruned) {
      const Lit pl = mkLit(*vi);
      const Lit nl = ~pl;
      debone(pl);
      debone(nl);
    }
    process_pruned_model(model);
  } else {
    process_pruned_model(model);
  }
}

void UpperBound::process_pruned_model(const vec<lbool>& model) {
  for (Var variable = 0; variable < model.size(); ++variable) {
    const lbool value = model[variable];
    const Lit   pos_lit = mkLit(variable);
    const Lit   neg_lit = ~mkLit(variable);
    if (value != l_True) might_be.remove(pos_lit);
    if (value != l_False) might_be.remove(neg_lit);
  }

  for (Var variable=model.size(); variable<=max_id; ++variable) {
    const Lit pos_lit = mkLit(variable);
    might_be.remove( pos_lit);
    might_be.remove(~pos_lit);
  }
}

/*-----------------------------  getters -------------------------------------*/
bool UpperBound::is_backbone(const Lit& literal) const { return might_be.get(literal); }
