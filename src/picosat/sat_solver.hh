//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        sat_solver.hh
 *
 * Description: Abstract SAT solver interface. TO BE INTEGRATED!
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _SAT_SOLVER_H
#define _SAT_SOLVER_H 1

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: SATSolver
 *
 * Purpose: Abstract interface for SAT solvers
\*----------------------------------------------------------------------------*/
//jpms:ec


class SATSolver {

public:

  virtual void init() = 0;

  virtual void reset() = 0;

  virtual int run_sat_solver() = 0;

  virtual set_clset(BasicClauseSet& _clset) = 0;

  virtual extract_core(BasicClauseVector& clvec) = 0;

  virtual extract_model() = 0;

};

#endif /* _SAT_SOLVER_H */

/*----------------------------------------------------------------------------*/
