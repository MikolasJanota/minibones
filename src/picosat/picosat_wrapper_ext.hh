//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_wrapper_ext.hh
 *
 * Description: 
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _PICOSAT_WRAPPER_EXT_H
#define _PICOSAT_WRAPPER_EXT_H 1

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

class PicosatWrapperExt {

public:
  PicosatWrapperExt(IDManager& _imgr, BasicClauseSet& _clset,
		    IntVector& _model, IntVector& _levels) :
    imgr(_imgr), clset(_clset), model(_model),
    levels(_levels), idcls() { }

  ~PicosatWrapperExt() { }

  int run_sat_solver();

  //void get_model(vector<int>& rmodel);

  void init();

  void reset();

  void get_var_levels(IntVector& varlevel);

  bool is_dvar(LINT vid);

  void print_cnf(const char* fname);

private:

  void update_maxvid(LINT nvid) {
    ULINT unvid = (nvid>0) ? nvid : -nvid;
    if (unvid > maxvid) { maxvid = unvid; } }

private:

  IDManager& imgr;                      // ID Manager

  BasicClauseSet& clset;                // Default clause set

  IntVector& model;                     // Array to store model

  IntVector& levels;                    // Array to store decision levels

  vector<BasicClause*> idcls;           // Clause associated with each ID

  //vector<int> model;                    // Computed model

  ULINT maxvid;

};

#endif /* _PICOSAT_WRAPPER_EXT_H */

/*----------------------------------------------------------------------------*/
