//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        core_importer.hh
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

#ifndef _CORE_IMPORTER_H
#define _CORE_IMPORTER_H 1

#include <vector>

#include "basic_clause.hh"
#include "basic_clset.hh"


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: 
 *
 * Purpose: 
\*----------------------------------------------------------------------------*/
//jpms:ec

class CoreImporter {

public:

  CoreImporter(BasicClauseSet& _clset, BasicClauseVector& _clvec) :
    clset(_clset), coreset(_clvec), clits() { }

  void reset() { clits.clear(); }

  void lit_importer(int lit) {
    //cout << "Importing lit " << lit << "...\n";
    if (lit) {
      clits.push_back(lit);
    } else {
      DBG(cout<<"Clause in unsat core: ";
	  for_each(clits.begin(),clits.end(),defprint<LINT>(cout));cout<<endl;
	  if (clits.size()==0){cout<<"Retrieving empty clause??\n";exit(3);});
      sort(clits.begin(), clits.end(), AbsLitLess());
      BasicClause* cl = clset.lookup(clits);
      assert(cl != NULL);
      coreset.push_back(cl);
      clits.clear();
    }
  }

protected:

  BasicClauseSet& clset;

  BasicClauseVector& coreset;

  IntVector clits;

};

#endif /* _CORE_IMPORTER_H */

/*----------------------------------------------------------------------------*/
