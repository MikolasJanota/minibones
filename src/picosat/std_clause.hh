/*----------------------------------------------------------------------------*\
 * Version: $Id: std_clause.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Interface between the generic (abstract) represesentation
 *              of clauses and literals and the clauses and literals in
 *              MiniSat.
 *
 *                               Copyright (c) 2005-2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _STD_CLAUSE_HH_
#define _STD_CLAUSE_HH_ 1

#include <iostream>

#include "SolverTypes.hh"    // MiniSat data types


/*----------------------------------------------------------------------------*\
 * Typedefs & Templates
\*----------------------------------------------------------------------------*/

typedef Clause* ClauseRef;    // Representation of clauses used by SAT solver

typedef RefSet<ClauseRef> ClRefSet;  // Set of cl refs

typedef Ref2IntMap<ClauseRef> ClIntMap;  // Map cl refs to ints

typedef ClIntMap ClIDMap;                // Map cl refs to cl IDs

template <class V>
class ClRefMap : public RefKeyMap<ClauseRef,V> { };


/*----------------------------------------------------------------------------*\
 * Class: StdClause
 *
 * Purpose: Class for interfacing minisat's clauses
\*----------------------------------------------------------------------------*/

class StdClause {

public:

  StdClause() : cl(NULL) { }

  inline void set_clause(Clause* _cl) { cl = _cl; }

  inline int size() const { return cl->size(); }
  inline bool learnt() const { return cl->learnt(); }
  inline int operator [] (int i) const {
    return (sign((*cl)[i])) ? -(var((*cl)[i])+1) : (var((*cl)[i])+1);
  }

public:

  void dump(ostream& outs) {
    int sz = size();
    outs<<"(";
    for(int i=0; i<sz; ++i) { outs<<" "<<lit(i); }
    cout<<" )"<<endl;
  }

  friend ostream & operator << (ostream& outs, StdClause& cl) {
    cl.dump(outs);
    return outs;
  }

protected:

  inline int lit(int i) {
    return (sign((*cl)[i])) ? -(var((*cl)[i])+1) : (var((*cl)[i])+1);
  }

protected:

  Clause* cl;

};

#endif /* _STD_CLAUSE_HH_ */

/*----------------------------------------------------------------------------*/
