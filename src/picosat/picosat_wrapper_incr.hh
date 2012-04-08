//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_wrapper_incr.hh
 *
 * Description: Incremental wrapper for the PicoSAT SAT solver.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _PICOSAT_WRAPPER_INCR_H
#define _PICOSAT_WRAPPER_INCR_H 1

#include "solver_wrapper.hh"


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: PicosatWrapperIncr
 *
 * Purpose: Provides incremental interface to the Picosat SAT solver
\*----------------------------------------------------------------------------*/
//jpms:ec

class PicosatWrapperIncr : public SATSolverWrapper {
  friend class SATSolverFactory;

public:

  // Direct interface

  void init_all();                  // Initialize all internal data structures

  void init_run();                  // Initialize data structures for SAT run

  SATRes solve();                   // Call SAT solver
  SATRes solve(const vector<LINT>& assumptions);

  void reset_all();                 // Clean up all internal data structures

  void reset_run();                 // Clean up data structures from SAT run


  // Config

  void set_verbosity(LINT verb) { verbosity = verb; }

  void set_phase(LINT ph) { phase = ph; }

  void set_literal_phase(LINT lit, int phase);


  // Access result of SAT solver call

  IntVector& get_model() { return model; }

  void get_model(IntVector& rmodel);

  BasicClauseVector& get_unsat_core() { assert(traceon); return ucore; }

  void get_unsat_core(BasicClauseVector& rucore);


  // Add/remove clauses or clause sets

  LINT size() {
    //assert(c2a_map.size() == a2c_map.size());
    return (LINT) c2a_map.size(); }

  void add_clause(BasicClause* cl) {
    //assert(c2a_map.find(cl) == c2a_map.end());
    CHK(if (c2a_map.find(cl) != c2a_map.end()) {
	cout << "Re-adding clause: " << *cl << endl; assert(0); });
    LINT aid = imgr.new_id();
    picosat_add_clause(cl, aid);
    //a2c_map.insert(make_pair(aid, cl));
    c2a_map.insert(make_pair(cl, -aid));  // Clause is active
  }

  void add_clauses(BasicClauseSet& rclset) {
    ClSetIterator cpos = rclset.begin();
    ClSetIterator cend = rclset.end();
    for(; cpos != cend; ++cpos) { add_clause(*cpos); }
  }

  bool exists_clause(BasicClause* cl) {
    return (c2a_map.find(cl) != c2a_map.end());
  }

  void replace_clause(BasicClause* cl) { del_clause(cl); add_clause(cl); }

  void del_clause(BasicClause* cl) {
    Cl2IntMap::iterator cpos = c2a_map.find(cl);
    assert(cpos != c2a_map.end());
    picosat_add_unit_clause(labs(cpos->second));  // Effectively block clause cl
    c2a_map.erase(cpos);
    //Int2ClMap::iterator apos = a2c_map.find(aid);
    //assert(apos != a2c_map.end());
    //a2c_map.erase(apos);
  }

  void del_all_clauses() {
    Cl2IntMap::iterator cpos = c2a_map.begin();
    Cl2IntMap::iterator cend = c2a_map.end();
    for(; cpos != cend; ++cpos) { picosat_add_unit_clause(labs(cpos->second)); }
    //a2c_map.clear();
    c2a_map.clear();
    /*
    IntVector assumes; assumes.resize(a2c_map.size(), NULL);
    copy(a2c_map.begin(), a2c_map.end(), assumes.begin());
    transform(assumes.begin(), assumes.end(), picosat_add_unit_clause); // ??
    */
  }

  void activate_clause(BasicClause* cl) {
    Cl2IntMap::iterator cpos = c2a_map.find(cl);
    assert(cpos != c2a_map.end());
    assert(cpos->second > 0);
    cpos->second = -cpos->second;  // Make assumption negative (non-satisfied)
  }

  void deactivate_clause(BasicClause* cl) {
    Cl2IntMap::iterator cpos = c2a_map.find(cl);
    assert(cpos != c2a_map.end());
    assert(cpos->second < 0);
    cpos->second = -cpos->second;  // Make assumption positive (satisfied)
  }

  void activate_all_clauses() {
    Cl2IntMap::iterator cpos = c2a_map.begin();
    Cl2IntMap::iterator cend = c2a_map.end();
    for(; cpos != cend; ++cpos) { activate_clause(cpos->first); }
  }

  void deactivate_all_clauses() {
    Cl2IntMap::iterator cpos = c2a_map.begin();
    Cl2IntMap::iterator cend = c2a_map.end();
    for(; cpos != cend; ++cpos) { deactivate_clause(cpos->first); }
  }

  void add_final_clause(BasicClause* cl) {
    assert(c2a_map.find(cl) == c2a_map.end());
    CHK(if (c2a_map.find(cl) != c2a_map.end()) {
	cout << "Re-adding clause: " << *cl << endl; assert(0); });
    picosat_add_clause(cl);
  }

  void add_final_clauses(BasicClauseSet& rclset) {
    ClSetIterator cpos = rclset.begin();
    ClSetIterator cend = rclset.end();
    for(; cpos != cend; ++cpos) { add_final_clause(*cpos); }
  }

  void make_clause_final(BasicClause* cl) {
    Cl2IntMap::iterator cpos = c2a_map.find(cl);
    assert(cpos != c2a_map.end());
    assert(cpos->second < 0);  // Confirm clause is active
    picosat_add_unit_clause(-labs(cpos->second));  // Cancel assumption literal
    c2a_map.erase(cpos);       // Remove assertion from consideration
  }

  //protected:
public:

  // Constructor/destructor

  PicosatWrapperIncr(IDManager& _imgr);

  virtual ~PicosatWrapperIncr();


protected:

  // Auxiliary functions

  void handle_sat_outcome();

  void handle_unsat_outcome();


protected:

  // Add clause to picosat, with associated assumption
  int picosat_add_clause(BasicClause* cl, LINT alit);

  int picosat_add_clause(BasicClause* cl);

  int picosat_add_unit_clause(LINT lit);

  void transmit_assumptions();

  // Compute unsat core given map of assumptions to clauses
  void compute_unsat_core();

  void print_cnf(const char* fname);

  inline void update_maxvid(LINT nvid) {
    ULINT unvid = (nvid>0) ? nvid : -nvid;
    if (unvid > maxvid) { maxvid = unvid; } }

protected:

  Cl2IntMap c2a_map;                 // Map from cls to assumptions

  //Int2ClMap a2c_map;                 // Map from assumptions to cls

  ULINT maxvid;

  LINT phase;

  LINT verbosity;

  bool isvalid;

};

#endif /* _PICOSAT_WRAPPER_INCR_H */

/*----------------------------------------------------------------------------*/
