/*----------------------------------------------------------------------------*\
 * Version: $Id: cnf_store.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Basic CNF store interface, guaranteed by SAT solvers
 *              used in NOTOS.
 *
 *                               Copyright (c) 2005-2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _CNF_STORE_HH_
#define _CNF_STORE_HH_ 1

#include <vector>

#include "globals.hh"
#include "std_clause.hh"

using namespace std;


/*----------------------------------------------------------------------------*\
 * Class: CNFStore
 *
 * Purpose: Abstract interface for consumers of CNF formulas.
\*----------------------------------------------------------------------------*/

class CNFStore {

public:

  CNFStore() { }                 // Default constructor

  virtual ~CNFStore() { }        // Virtual destructor

  //----------------------------------------------------------------------------
  // Basic interface
  //----------------------------------------------------------------------------

  virtual int new_var() = 0;     // Obtain identifier for new variable
  virtual int num_vars() = 0;    // Return number of existing variables

  virtual ClauseRef add_clause(vector<int>& vlit) = 0;  // Create cl w/ lit vect
  virtual int num_base_cls() = 0;     // Return number of original clauses
  virtual int num_learnt_cls() = 0;   // Return number of learnt clauses
  virtual int num_total_cls() = 0;    // Return total number of clauses

  virtual StdClause& get_clause(ClauseRef clref) = 0;  // Get clause clref

  //----------------------------------------------------------------------------
  // Interface with incrementality and shadowing
  //----------------------------------------------------------------------------

  // ...

};

#endif /* _CNF_STORE_HH_ */

/*----------------------------------------------------------------------------*/
