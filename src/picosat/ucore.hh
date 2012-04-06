/*----------------------------------------------------------------------------*\
 * File:        ucore.hh
 *
 * Description: UnsatCore class. The purpose of this class is to generate
 *              an unsatisfiable core from a proof trace.
 *
 * Author:      jpms
 * 
 * Revision:    $Id: ucore.hh 73 2007-07-26 15:16:48Z jpms $.
 *
 *                                     Copyright (c) 2007, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _UCORE_H
#define _UCORE_H 1

#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <cassert>

#include <vector>
#include <set>
#include <ext/hash_map>       // Location of STL hash extensions

using namespace std;
using namespace __gnu_cxx;    // Required for STL hash extensions


#include "globals.hh"
#include "types.hh"

#include "cnf_store.hh"

class ProofTrace;
class Clause;

typedef RefSet<Clause*> ClSet;


/*----------------------------------------------------------------------------*\
 * Class: UnsatCore
 *
 * Purpose: Identified clauses in unsat core, given proof trace.
\*----------------------------------------------------------------------------*/

class UnsatCore {

public:

  UnsatCore() : clrefs() { }

  ~UnsatCore() { clrefs.clear(); }

  ClSet& compute_core(ProofTrace& ptrace);

  inline ClSet& cl_refs() { return clrefs; }

  void dump(ostream& outs);

  friend ostream & operator << (ostream& outs, UnsatCore& uc) {
    uc.dump(outs);
    return outs;
  }

protected:
  ClSet clrefs;    // Clauses in unsat core

};

#endif /* _UCORE_H */

/*----------------------------------------------------------------------------*/
