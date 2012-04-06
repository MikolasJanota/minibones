//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_bridge_sort.hh
 *
 * Description: 
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2011, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _PICOSAT_BRIDGE_SORT_H
#define _PICOSAT_BRIDGE_SORT_H 1

#include <stdio.h>

#include "globals.hh"
#include "id_manager.hh"
#include "basic_clause.hh"
#include "basic_clset.hh"


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: 
 *
 * Purpose: 
\*----------------------------------------------------------------------------*/
//jpms:ec

class PicosatWrapperSort {

public:
  PicosatWrapperSort(IDManager& _imgr, BasicClauseSet& _clset,
		     IntVector& _model, BasicClauseVector& _clvec) :
    imgr(_imgr), clset(_clset), model(_model), clvec(_clvec),
    idcls(), trace_file(NULL), niter(0), unique_file(false) { }

  ~PicosatWrapperSort() { }

  int run_sat_solver(vector<LINT> & priority_lits);

  int run_sat_solver();

  //void get_model(vector<int>& rmodel);

  void init();

  void reset();

  void get_var_levels(IntVector& varlevel);

  bool is_dvar(LINT vid);

  void print_cnf(const char* fname);

  void print_trace();

  void set_trace(const char* fname) { trace_file = fname; }

  void set_trace(const char* fname, bool unique) { trace_file = fname; unique_file=unique; }

protected:

  void compute_unsat_core();

private:

  void update_maxvid(LINT nvid) {
    ULINT unvid = (nvid>0) ? nvid : -nvid;
    if (unvid > maxvid) { maxvid = unvid; } }

private:

  IDManager& imgr;                      // ID Manager

  BasicClauseSet& clset;                // Default clause set

  IntVector& model;                     // Array to store model

  BasicClauseVector& clvec;             // Vector for unsat core

  vector<BasicClause*> idcls;           // Clause associated with each ID

  //vector<int> model;                    // Computed model

  ULINT maxvid;

  const char* trace_file;

  long int niter;

  bool unique_file;

};

#endif /* _PICOSAT_BRIDGE_SORT_H */

/*----------------------------------------------------------------------------*/
