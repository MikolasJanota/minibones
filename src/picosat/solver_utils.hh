//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        solver_utils.hh
 *
 * Description: Utilities used in interfacing SAT solvers.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _SOLVER_UTILS_H
#define _SOLVER_UTILS_H 1

namespace SolverUtils {

  template <typename S, typename T>
  void add_clauses(S& solver,
		   typename T::iterator ccpos, typename T::iterator ccend) {
    for(; ccpos != ccend; ++ccpos) { solver.add_clause(*ccpos); }
  }

  template <typename S, typename T>
  void del_clauses(S& solver, typename T::iterator ccpos,
		   typename T::iterator ccend) {
    for(; ccpos != ccend; ++ccpos) { solver.del_clause(*ccpos); }
  }

  template <typename S, typename T>
  void activate_clauses(S& solver, typename T::iterator ccpos,
			typename T::iterator ccend) {
    for(; ccpos != ccend; ++ccpos) { solver.activate_clause(*ccpos); }
  }

  template <typename S, typename T>
  void deactivate_clauses(S& solver, typename T::iterator ccpos,
			  typename T::iterator ccend) {
    for(; ccpos != ccend; ++ccpos) { solver.deactivate_clause(*ccpos); }
  }

}

#endif /* _SOLVER_UTILS_H */

/*----------------------------------------------------------------------------*/
