//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        solver_config.hh
 *
 * Description: 
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _SOLVER_CONFIG_H
#define _SOLVER_CONFIG_H 1

#include "id_manager.hh"


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: 
 *
 * Purpose: 
\*----------------------------------------------------------------------------*/
//jpms:ec

class SATSolverConfig {

public:

  SATSolverConfig() { }

  virtual ~SATSolverConfig() { }

  virtual bool chk_sat_solver(const char* _solver) = 0;

  virtual bool get_incr_mode() = 0;

  virtual int get_verbosity() = 0;

};

#endif /* _SOLVER_CONFIG_H */

/*----------------------------------------------------------------------------*/
