//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_wrapper_nonincr.hh
 *
 * Description: Non-incremental wrapper for PicoSAT SAT solver.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _PICOSAT_WRAPPER_NONINCR_H
#define _PICOSAT_WRAPPER_NONINCR_H 1

#include "solver_wrapper.hh"


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: PicosatWrapperNonIncr
 *
 * Purpose: Provides non-incremental interface to Picosat SAT solver
\*----------------------------------------------------------------------------*/
//jpms:ec

class PicosatWrapperNonIncr : public SATSolverWrapper {
  friend class SATSolverFactory;

public:

  // Direct interface

  virtual void init_all();        // Initialize all internal data structures

  virtual void init_run();         // Initialize data structures for SAT run

  virtual SATRes solve();

  virtual void reset_all();         // Clean up all internal data structures

  virtual void reset_run();         // Clean up data structures from SAT run


  // Config

  void set_verbosity(LINT verb) { verbosity = verb; }

  void set_phase(LINT ph) { phase = ph; }

  void set_literal_phase(LINT lit, int phase);


  // Access result of SAT solver call

  IntVector& get_model() { return model; }

  void get_model(IntVector& rmodel);

  BasicClauseVector& get_unsat_core() { assert(traceon); return ucore; }

  void get_unsat_core(BasicClauseVector& rucore);


  // Manipulate local copy of clause set

  LINT size() { return (LINT) clset.size(); }

  // Add clause to local clset
  void add_clause(BasicClause* cl) {
    assert(clset.find(cl) == clset.end());
    clset.insert(cl);
  }

  // Add all clauses in clset to local clset
  void add_clauses(BasicClauseSet& rclset) {
    ClSetIterator cpos = rclset.begin();
    ClSetIterator cend = rclset.end();
    for(; cpos != cend; ++cpos) { add_clause(*cpos); }
  }

  // Check if clause exists
  bool exists_clause(BasicClause* cl) {
    return (clset.find(cl) != clset.end());
  }

  // Replace clause in local clset
  void replace_clause(BasicClause* cl) { del_clause(cl); add_clause(cl); }

  // Delete clause from local clset
  void del_clause(BasicClause* cl) {
    DBG(cout << "ClSet size A: " << clset.size() << endl;);
    assert(clset.find(cl) != clset.end());
    clset.erase(cl);
    DBG(cout << "ClSet size B: " << clset.size() << endl;);
  }

  // Remove all clauses from local clset
  void del_all_clauses() { clset.clear(); }


  // Activate clause (assumed deactivated)
  void activate_clause(BasicClause* cl) {
    assert(dactiv.find(cl) != dactiv.end());
    dactiv.erase(cl); clset.insert(cl); 
  }

  // Deactivate clause (assumed activated)
  void deactivate_clause(BasicClause* cl) {
    assert(clset.find(cl) != clset.end());
    clset.erase(cl); dactiv.insert(cl);
  }

  void activate_all_clauses() {
    assert(clset.size() == 0);
    HashedClauseSet::iterator cpos = dactiv.begin();
    HashedClauseSet::iterator cend = dactiv.end();
    for(; cpos != cend; ++cpos) { clset.insert(*cpos); }
    dactiv.clear();
  }

  void deactivate_all_clauses() {
    assert(dactiv.size() == 0);
    HashedClauseSet::iterator cpos = clset.begin();
    HashedClauseSet::iterator cend = clset.end();
    for(; cpos != cend; ++cpos) { dactiv.insert(*cpos); }
    clset.clear();
  }

protected:
  //public:

  // Constructor/destructor

  PicosatWrapperNonIncr(IDManager& _imgr) :
    SATSolverWrapper(_imgr), clset(), dactiv(), idcls() {
    maxvid = 0; phase = 2; verbosity = 0; initok = false; }

  virtual ~PicosatWrapperNonIncr() { }


protected:

  // Auxiliary functions

  virtual void handle_sat_outcome();

  virtual void handle_unsat_outcome();


protected:

  void print_cnf(const char* fname);

  inline void update_maxvid(LINT nvid) {
    ULINT unvid = (nvid>0) ? nvid : -nvid;
    if (unvid > maxvid) { maxvid = unvid; } }

protected:

  HashedClauseSet clset;

  HashedClauseSet dactiv;

  BasicClauseVector idcls;           // Clause associated with each ID

  ULINT maxvid;

  LINT phase;

  LINT verbosity;

  bool initok;

};

#endif /* _PICOSAT_WRAPPER_NONINCR_H */

/*----------------------------------------------------------------------------*/
