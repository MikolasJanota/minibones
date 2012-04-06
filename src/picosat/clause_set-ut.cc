/*----------------------------------------------------------------------------*\
 * Version: $Id: clause_set-ut.cc 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Unit tests for the clause set class.
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#ifdef __UNIT_TEST__

#include "clause_set.hh"

#include "cnf_store_stub.hh"


/*----------------------------------------------------------------------------*\
 * Unit tests for target class
\*----------------------------------------------------------------------------*/

#include "TestHarness.h"


/*----------------------------------------------------------------------------*\
 * Unit tests for the ClauseSet class
\*----------------------------------------------------------------------------*/

TC( ClauseSet, simple_tests )
{
  logtc();    // Prints test group and name when running test

  ClauseSet clset;

  int clid = clset.add_clause();
  CHECK(clid == 1);

  clset.add_literal(clid, -1);
  clset.add_literal(clid, 2);
  clset.add_literal(clid, -3);
  CHECK(clset.size() == 1);

  ClauseSet::iterator cpos = clset.begin();
  PlainClause* cl = *cpos;
  PlainClause::iterator lpos = cl->begin();
  CHECK(*lpos == -1);
  lpos++;
  CHECK(*lpos == 2);
  lpos++;
  CHECK(*lpos == -3);
  lpos++;
  CHECK(lpos == cl->end());

  CNFStoreStub cnff;
  IDMap cnfids;
  ClSetExporter cexp(clset);
  ClauseSetUtils::export_cnf(cexp, cnff, cnfids);
  cout << "Formula: \n" << cnff << endl;

  clset.clear();
  CHECK(clset.size() == 0);

  clid = clset.add_clause();
  CHECK(clid == 1);

  clset.add_literal(clid, -1);
  clset.add_literal(clid, 2);
  clset.add_literal(clid, -3);
  CHECK(clset.size() == 1);

  cpos = clset.begin();
  cl = *cpos;
  lpos = cl->begin();
  CHECK(*lpos == -1);
  lpos++;
  CHECK(*lpos == 2);
  lpos++;
  CHECK(*lpos == -3);
  lpos++;
  CHECK(lpos == cl->end());
}

// Add test cases...


SimpleString StringFrom(const std::string& value)
{
  return SimpleString(value.c_str());
}

// Main function, used to run the unit tests.
int main(int argc, char** argv)
{
  TestResult tr;
  int nf = TestRegistry::runAllTests(tr);

  const char *fmsg = (nf == 0) ? "passed. " : "failed. ";
  cout << "Unit testing of class ClauseSet " << fmsg;
  cout << "Number of failed unit tests: " << nf << ".\n";
  return 0;
}

#endif // __UNIT_TEST__


/*----------------------------------------------------------------------------*/
