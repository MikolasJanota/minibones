//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        solver_wrapper.hh
 *
 * Description: General SAT solver wrapper & SAT solver factory.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _SOLVER_WRAPPER_H
#define _SOLVER_WRAPPER_H 1

#include "globals.hh"
#include "logger.hh"
#include "id_manager.hh"
#include "basic_clset.hh"
#include "solver_utils.hh"

using namespace SolverUtils;


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Basic defs
\*----------------------------------------------------------------------------*/
//jpms:ec

typedef enum SAT_Result {
  SAT_NoRes = 0x0000,    // Default value: no result
  SAT_True  = 0x2000,    // Instance is satisfiable
  SAT_False = 0x2001,    // Instance is unsatisfiable
  SAT_Abort = 0x2002,    // Resources exceeded
} SATRes;

class SATSolverFactory;


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: SATSolverWrapper
 *
 * Purpose: Provides configurable interface to SAT solver
\*----------------------------------------------------------------------------*/
//jpms:ec

class SATSolverWrapper {
  friend class SATSolverFactory;

public:

  // Direct interface

  virtual void init_all() = 0;        // Initialize all internal data structures

  virtual void init_run() = 0;         // Initialize data structures for SAT run

  virtual SATRes solve() = 0;

  virtual void reset_all() = 0;         // Clean up all internal data structures

  virtual void reset_run() = 0;         // Clean up data structures from SAT run

  void set_unsat_trace(bool tracef) { traceon = tracef; }

  void set_verbosity(int verb) { verbosity = verb; }


  // Access result of SAT solver call

  virtual IntVector& get_model() = 0;

  virtual void get_model(IntVector& rmodel) = 0;

  virtual BasicClauseVector& get_unsat_core() = 0;

  virtual void get_unsat_core(BasicClauseVector& rucore) = 0;


  // Manipulate local copy of clause set

  virtual LINT size() = 0;

  virtual void add_clause(BasicClause* cl) = 0;

  virtual void add_clauses(BasicClauseSet& rclset) = 0;

  virtual bool exists_clause(BasicClause* cl) = 0;

  virtual void replace_clause(BasicClause* cl) = 0;

  virtual void del_clause(BasicClause* cl) = 0;

  virtual void del_all_clauses() = 0;

  virtual void activate_clause(BasicClause* cl) = 0;

  virtual void deactivate_clause(BasicClause* cl) = 0;

  virtual void activate_all_clauses() = 0;

  virtual void deactivate_all_clauses() = 0;

  // Final clauses cl cannot be deactivated, deleted or replaced

  virtual void add_final_clause(BasicClause* cl) { }

  virtual void add_final_clauses(BasicClauseSet& rclset) { }

  virtual void make_clause_final(BasicClause* cl) { }

  // Managing groups of clauses

  virtual LINT make_group() { return 0; }

  virtual void add_clause(BasicClause* cl, LINT groupid) { }

  virtual void activate_group(LINT groupid) { }

  virtual void deactivate_group(LINT groupid) { }

  virtual void del_group(LINT groupid) { }

  // Configure the solver

  virtual void set_literal_phase(LINT lit, int phase) = 0;


protected:

  // Constructor/destructor

  SATSolverWrapper(IDManager& _imgr) :
    imgr(_imgr), model(), ucore(), verbosity(0), traceon(false) { }

  virtual ~SATSolverWrapper() { }


  // Auxiliary functions

  virtual void handle_sat_outcome() = 0;

  virtual void handle_unsat_outcome() = 0;


protected:

  IDManager& imgr;

  IntVector model;

  BasicClauseVector ucore;

  int verbosity;

  bool traceon;

};


#endif /* _SOLVER_WRAPPER_H */

/*----------------------------------------------------------------------------*/
