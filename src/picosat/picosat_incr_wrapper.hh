//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_incr_wrapper.hh
 *
 * Description: Incremental interface for running picosat.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _PICOSAT_INCR_WRAPPER_H
#define _PICOSAT_INCR_WRAPPER_H 1

#include <stdio.h>
#include <stdlib.h>

#include "globals.hh"
#include "basic_clause.hh"
#include "basic_clset.hh"


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: PicosatIncrementalWrapper
 *
 * Purpose: Incremental wrapper for running picosat.
\*----------------------------------------------------------------------------*/
//jpms:ec

class PicosatIncrementalWrapper {

public:
  PicosatIncrementalWrapper() : idcls() {
    iclset = NULL; iclvect = NULL; iassume = NULL;
    ret_vect = NULL; corecls = NULL;
    varlim = LONG_MAX; verbosity = 0; phase = 2; ccls = false; }

  ~PicosatIncrementalWrapper() { }

  //jpms:bc
  //----------------------------------------------------------------------------
  // Run SAT solver given incremental changes to previous clsets
  // _iclset: incremental changes to existing clset
  // _iassume: assumptions configuring CNF formula
  // _ret_vect: return object, either model or core assumptions
  // Return value: unsat (20), sat (10), etc.
  //----------------------------------------------------------------------------
  //jpms:ec

  int solve(BasicClauseSet& _iclset, IntVector& _iassume, IntVector& _ret_vect);

  int solve(BasicClauseVector& _iclvect,
	    IntVector& _iassume, IntVector& _ret_vect);

  int solve(BasicClauseSet& _iclset, IntVector& _iassume,
	    IntVector& _ret_vect, BasicClauseSet& _corecls);

  int solve(BasicClauseVector& _iclvect, IntVector& _iassume,
	    IntVector& _ret_vect, BasicClauseSet& _corecls);

  void init(bool traceon = false);

  void reset();

  void set_top_var_thres(LINT nvar) { assert(nvar > 0); varlim = nvar; }

  void set_verbosity(LINT verb) { verbosity = verb; }

  void set_phase(LINT ph) { phase = ph; }

  void set_core_cls() { ccls = true; }

  void print_cnf(const char* fname);

  void get_var_levels(IntVector& varlevel);

  bool is_dvar(LINT vid);

protected:

  void transmit_clauses_set();

  void transmit_clauses_vect();

  void transmit_assumptions();

  void handle_unsat_outcome();

  void handle_sat_outcome();

  void get_core_assumptions();

  void get_core_clauses();

private:

  void update_maxvid(LINT nvid) {
    ULINT unvid = (nvid>0) ? nvid : -nvid;
    if (unvid > maxvid) { maxvid = unvid; } }

private:

  BasicClauseSet* iclset;

  BasicClauseVector* iclvect;

  BasicClauseSet* corecls;

  IntVector* iassume;

  IntVector* ret_vect;

  vector<BasicClause*> idcls;           // Clause associated with each ID

  ULINT maxvid;

  LINT varlim;

  LINT verbosity;

  LINT phase;

  bool ccls;

};

#endif /* _PICOSAT_INCR_WRAPPER_H */

/*----------------------------------------------------------------------------*/
