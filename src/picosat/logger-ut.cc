/*----------------------------------------------------------------------------*\
 * Version: $Id: logger-ut.cc 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: 
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#include "logger.hh"

/*----------------------------------------------------------------------------*/
#ifdef __UNIT_TEST__

/*----------------------------------------------------------------------------*\
 * Unit tests for target class
\*----------------------------------------------------------------------------*/

#include "TestHarness.h"

using namespace LogUtils;


TC( LogUtils, simple_test )
{
  logtc();

  TLOG(set_log_fname("test-log"););
  TLOG(tlog << "Testing a count of " << 125 << endl;);
  TLOG(log_cleanup(););
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
  cout << "Unit testing of class Logger " << fmsg;
  cout << "Number of failed unit tests: " << nf << ".\n";
  return 0;
}

#endif // __UNIT_TEST__


/*----------------------------------------------------------------------------*/
