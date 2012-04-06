/*----------------------------------------------------------------------------*\
 * Version: $Id: cnf_dumper.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: A dump utility for CNF formulas.
 *
 *                               Copyright (c) 2005-2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _CNF_DUMPER_HH_
#define _CNF_DUMPER_HH_ 1

#include <vector>

using namespace std;


#include "globals.hh"

#include "std_clause.hh"

#include "cnf_store.hh"


class CNFDumper : public CNFStore {

public:

  CNFDumper() : CNFStore(), clset(), stdcl(), nvars(0) { }
  virtual ~CNFDumper() { clset.clear(); }

  friend ostream & operator << (ostream& outs, CNFDumper& cnf) {
    cnf.dump(outs);
    return outs;
  }

  virtual void dump(ostream& outs) {
    outs << "p cnf " << nvars << " " << clset.size() << endl;
    vector<vector<int> >::iterator cpos = clset.begin();
    vector<vector<int> >::iterator cend = clset.end();
    for (; cpos != cend; ++cpos) {
      vector<int>& clref = *cpos;
      vector<int>::iterator ipos = clref.begin();
      vector<int>::iterator iend = clref.end();
      for(; ipos != iend; ++ipos) {
	outs << *ipos << " ";
      }
      outs << "0" << endl;
    }
  }

  //----------------------------------------------------------------------------
  // Basic interface
  //----------------------------------------------------------------------------

  int new_var()  { return ++nvars; }    // Obtain identifier for new variable
  int num_vars() { return nvars; }      // Return number of existing variables

  ClauseRef add_clause(vector<int>& vlit) {  // Create cl w/ vect of lits
    clset.push_back(vlit);
    return NULL;
  }

  // Return number of original clauses
  int num_base_cls() { return clset.size(); }
  // Return number of learnt clauses
  int num_learnt_cls() { assert(0); return 0; }
  int num_total_cls() { return clset.size(); } // Return total number of clauses
  
  // Get clause clid
  StdClause& get_clause(ClauseRef clref) { assert(0); return stdcl; }

protected:

  vector<vector<int> > clset;

  StdClause stdcl;

  int nvars;

};

#endif /* _CNF_DUMPER_HH_ */

/*----------------------------------------------------------------------------*/
