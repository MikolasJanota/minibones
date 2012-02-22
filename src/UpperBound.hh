/*
 * File:  UpperBound.hh
 * Author:  mikolas
 * Created on:  Tue, Feb 21, 2012 3:44:59 PM
 * Copyright (C) 2012, Mikolas Janota
 */
#ifndef UPPERBOUND_HH_28341
#define UPPERBOUND_HH_28341
#include <vector>
#include <functional>
#include "core/Solver.h"
#include "auxiliary.hh"
#include "Lifter.hh"
#include "LitBitSet.hh"
#include "ToolConfig.hh"
#include "Rotatable.hh"
#include "BackboneInformation.hh"
namespace minibones {
  using Minisat::Solver;
  using Minisat::Var;
  using Minisat::Lit;
  using Minisat::vec;

  /** Class for computing backbones with the upper bound approach. */
  class UpperBound : public BackboneInformation {
  public:
    UpperBound(ToolConfig& tool_configuration, ostream& output, Var max_id, const CNF& clauses);
    virtual ~UpperBound();
  public:
    /** Initialize the worker, returns true iff the instance is SAT. */
    bool initialize();
    /** Start the worker, initialize must be called first. */
    void run();
    virtual bool is_backbone(const Lit& literal) const;
  private:// initial
    const ToolConfig&   tool_configuration;
    int                 chunk_size;
    ostream&            output;
    const Var           max_id;
    const CNF&          clauses;
    const Range         variable_range;
  private:// state
    LitBitSet           might_be; // literals that might still be backbones   
    LitBitSet           must_be;  // literals that must be backbones   
  private:// stats
    UINT                solver_calls;  // number of solver calls, for statistical purposes
  private:// sub-objects
    Solver              solver;
    Lifter              lifter;               // used to reduce models via lifting
    Rotatable           rotatable_computer;   // used to get rotatable variables
  private:// backbone testing
    /** Debones everything not in the model.*/
    void process_model(const vec<lbool>& model);
    void process_pruned_model(const vec<lbool>& model);
    inline bool debone(Lit literal);
  };

  inline bool UpperBound::debone(Lit literal) { return might_be.remove (literal); }
} /* end of namespace minibones */
#endif /* UPPERBOUND_HH_28341 */
