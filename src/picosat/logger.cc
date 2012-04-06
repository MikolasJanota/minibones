/*----------------------------------------------------------------------------*\
 * Version: $Id: logger.cc 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Implementation of the logging utilities.
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#include "logger.hh"


using namespace std;


ostream *Logger::logf = NULL;

ostream *Logger::errf = NULL;

int Logger::verbosity = 1;


void LogUtils::set_log_fname(const char* fname)
{
  Logger::logf = (ostream*) new ofstream(fname);
}

void LogUtils::set_log_verbosity(int verb)
{
  Logger::verbosity = verb;
}

void LogUtils::set_error_stream(ostream *err)
{
  Logger::errf = err;
}

void LogUtils::log_cleanup()
{
  if (Logger::logf) {
    (*Logger::logf).flush(); delete Logger::logf; Logger::logf = NULL;
  }
}

using namespace XRT;

void LogUtils::set_tool_log_names(string tn, string nst)
{
  XRT::set_tname(tn);
  XRT::set_stname(nst);
}

/*----------------------------------------------------------------------------*/
