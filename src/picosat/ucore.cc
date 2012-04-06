/*----------------------------------------------------------------------------*\
 * File:        ucore.cc
 *
 * Description: Implementation of class UnsatCore, for computing
 *              unsatisfiable formula from a proof trace.
 *
 * Author:      jpms
 * 
 * Revision:    $Id: ucore.cc 73 2007-07-26 15:16:48Z jpms $.
 *
 *                                     Copyright (c) 2007, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#include <set>
#include <ext/hash_map>       // Location of STL hash extensions

using namespace std;
using namespace __gnu_cxx;    // Required for STL hash extensions

#include "globals.hh"

#include "ptrace.hh"

#include "ucore.hh"


/*----------------------------------------------------------------------------*\
 * Purpose: Compute the unsat core
\*----------------------------------------------------------------------------*/

ClSet& UnsatCore::compute_core(ProofTrace& _ptrace)
{
  NDBG(cout<<"Computing res proof cls..."<<endl;);
  _ptrace.begin_traversal(&clrefs);
  while(_ptrace.has_cl2traverse()) {
    _ptrace.update_cl2traverse();
    DBG(ClauseRef clref = (ClauseRef))
    _ptrace.curr_traced_clause();  // Do nothing
    DBG(cout << "Tracing cl: " << *clref << endl;);
    _ptrace.next_cl2traverse();    // Go over deps; decide which cls to visit
  }
  _ptrace.end_traversal();
  NDBG(cout<<"Done computing res proof cls..."<<endl;);
  return clrefs;
}

void UnsatCore::dump(ostream& outs)
{
  outs << clrefs;
}

/*----------------------------------------------------------------------------*/
