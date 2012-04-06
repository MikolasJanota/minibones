/*----------------------------------------------------------------------------*\
 * Version: $Id: ptrace-ut.cc 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Unit tests for proof tracing class.
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#include "ptrace.hh"


/*----------------------------------------------------------------------------*/
#ifdef __UNIT_TEST__

/*----------------------------------------------------------------------------*\
 * Unit tests for target class
\*----------------------------------------------------------------------------*/

#include "TestHarness.h"

TC( ProofTrace, trace_single_cl )  // Test trace of a single clause
{
  logtc();    // Prints test group and name when running test
  ProofTrace pt;

  pt.begin_cl_trace();
  pt.add_dep_pair((ClauseRef)0x001, 0);    // Unsat clause; no var
  pt.add_dep_pair((ClauseRef)0x002, 3);
  pt.add_dep_pair((ClauseRef)0x003, -1);
  pt.add_dep_pair((ClauseRef)0x004, 2);
  pt.add_dep_pair((ClauseRef)0x007, -6);
  pt.add_dep_pair((ClauseRef)0x006, 5);
  pt.add_dep_pair((ClauseRef)0x005, -4);
  pt.end_cl_trace((ClauseRef)0x008);

  DBG(cout << pt;);

  pt.begin_traversal();
  CHECK(pt.has_cl2traverse());  // Must traverse one clause
  pt.update_cl2traverse();
  ClauseRef curr_cl = pt.curr_traced_clause();
  ClDepPair* cdeps = pt.curr_traced_cldeps();
  pt.next_cl2traverse();
  CHECK(!pt.has_cl2traverse());
  pt.end_traversal();

  pt.init_trace_cl_deps(curr_cl, cdeps);

  CHECK(pt.has_dep_pair());
  pt.update_dep_pair();
  CHECK(pt.curr_lit() == -4 && pt.curr_cl() == (ClauseRef)0x005);
  pt.next_dep_pair();

  CHECK(pt.has_dep_pair());
  pt.update_dep_pair();
  CHECK(pt.curr_lit() == 5 && pt.curr_cl() == (ClauseRef)0x006);
  pt.next_dep_pair();

  CHECK(pt.has_dep_pair());
  pt.update_dep_pair();
  CHECK(pt.curr_lit() == -6 && pt.curr_cl() == (ClauseRef)0x007);
  pt.next_dep_pair();

  CHECK(pt.has_dep_pair());
  pt.update_dep_pair();
  CHECK(pt.curr_lit() == 2 && pt.curr_cl() == (ClauseRef)0x004);
  pt.next_dep_pair();

  CHECK(pt.has_dep_pair());
  pt.update_dep_pair();
  CHECK(pt.curr_lit() == -1 && pt.curr_cl() == (ClauseRef)0x003);
  pt.next_dep_pair();

  CHECK(pt.has_dep_pair());
  pt.update_dep_pair();
  CHECK(pt.curr_lit() == 3 && pt.curr_cl() == (ClauseRef)0x002);
  pt.next_dep_pair();

  CHECK(pt.has_dep_pair());
  pt.update_dep_pair();
  CHECK(pt.curr_lit() == 0 && pt.curr_cl() == (ClauseRef)0x001);
  pt.next_dep_pair();

  CHECK(!pt.has_dep_pair());

  // ...
}

#include "Solver.hh"

TC( ProofTrace, sm_proof_trace_ex1 )  // Test trace of a single clause
{
  logtc();    // Prints test group and name when running test
  ProofTrace pt;
  Solver ms;
  ms.register_proof_logger(&pt);

  vector<int> cl;
  cl.push_back(1);
  cl.push_back(3);
  ms.add_clause(cl);
  cl.clear();
  cl.push_back(-1);
  cl.push_back(2);
  ms.add_clause(cl);
  cl.clear();
  cl.push_back(-3);
  cl.push_back(4);
  ms.add_clause(cl);
  cl.clear();
  cl.push_back(-3);
  cl.push_back(-4);
  ms.add_clause(cl);
  cl.clear();
  cl.push_back(-2);
  ms.add_clause(cl);
  cl.clear();

  ms.solve();
  CHECK(ms.okay() == false);

  DBG(cout << pt);

  // ...
}

TC( ProofTrace, sm_proof_trace_ex2 )  // Test trace of a single clause
{
  logtc();    // Prints test group and name when running test
  ProofTrace pt;
  Solver ms;
  ms.register_proof_logger(&pt);

  vector<int> cl;
  cl.push_back(1);
  cl.push_back(2);
  ms.add_clause(cl);
  cl.clear();
  cl.push_back(1);
  cl.push_back(-2);
  ms.add_clause(cl);
  cl.clear();
  cl.push_back(-1);
  cl.push_back(3);
  ms.add_clause(cl);
  cl.clear();
  cl.push_back(-1);
  cl.push_back(-3);
  ms.add_clause(cl);
  cl.clear();

  ms.solve();
  CHECK(ms.okay() == false);

  DBG(cout << pt);

  // ...
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
  cout << "Unit testing of class ProofTrace " << fmsg;
  cout << "Number of failed unit tests: " << nf << ".\n";
  return 0;
}

#endif // __UNIT_TEST__

/*----------------------------------------------------------------------------*/
