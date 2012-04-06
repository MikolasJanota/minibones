//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        solver_factory.hh
 *
 * Description: SAT solver object factory.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _SOLVER_FACTORY_H
#define _SOLVER_FACTORY_H 1

#include "solver_config.hh"
#include "solver_wrapper.hh"
#include "picosat_wrapper_nonincr.hh"
#include "picosat_wrapper_incr.hh"


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: SATSolverFactory
 *
 * Purpose: Creates a SAT solver object given configuration
\*----------------------------------------------------------------------------*/
//jpms:ec

class SATSolverFactory {

public:

  SATSolverFactory(IDManager& _imgr) : imgr(_imgr), solver(NULL) { }

  virtual ~SATSolverFactory() {
    if (solver != NULL) { delete solver; solver = NULL; } }

  SATSolverWrapper& instance(SATSolverConfig& config) {
    if (solver != NULL) { return *solver; }
    if (config.get_incr_mode()) {                     // Run in incremental mode
      if (config.chk_sat_solver("picosat")) {
	      solver = (SATSolverWrapper*) new PicosatWrapperIncr(imgr);
      }
      else {
	      tool_abort("Selection of invalid incremental SAT solver in factory");
      }
    }
    else {
      if (config.chk_sat_solver("picosat")) {
	      solver = (SATSolverWrapper*) new PicosatWrapperNonIncr(imgr);
      }
      else {
	      tool_abort("Selection of invalid non-incremental SAT solver in factory");
      }
    }
    solver->set_verbosity(config.get_verbosity());
    return *solver;
  }

  void release() { if (solver != NULL) { delete solver; solver = NULL; } }

protected:

  IDManager& imgr;

  SATSolverWrapper* solver;

};

#endif /* _SOLVER_FACTORY_H */

/*----------------------------------------------------------------------------*/
