/*----------------------------------------------------------------------------*\
 * Version: $Id: logger.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Declaration of the logging utilities.
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _LOGGER_HH_
#define _LOGGER_HH_ 1

#include <fstream>
#include <iostream>


using namespace std;


#include "globals.hh"

#include "runtime_msgs.hh"


namespace LogUtils {

  void set_log_fname(const char* fname);

  void set_log_verbosity(int verb);

  int get_log_verbosity();

  void set_error_stream(ostream *err);

  void log_cleanup();

  void set_tool_log_names(string tn, string nst);

};


class Logger {

  friend void LogUtils::set_log_fname(const char* fname);

  friend void LogUtils::set_log_verbosity(int verb);

  friend int LogUtils::get_log_verbosity() { return verbosity; }

  friend void LogUtils::set_error_stream(ostream *err);

  friend void LogUtils::log_cleanup();

  //friend ostream& tlog() { return *logf; }

public:

  static int verbosity;

  static ostream *logf;

  static ostream *errf;

};

/*----------------------------------------------------------------------------*\
 * Print/Log macros
\*----------------------------------------------------------------------------*/

// Use tlog instead of (*Logger::logf)
#define tlog (*Logger::logf)

// Print subject
#define COND_PRT(cmd,verb) {if (verb <= LogUtils::get_log_verbosity()) { cmd; }}

// Activate or deactivate logging: define the tool log macro LOGGING
#ifdef LOGGING
#define TLOG(x) x
#else
#define TLOG(x) 
#endif


// File logs corresponds to the standard tool log
#define flog tlog
#define FLOG(x) TLOG(x)


// Use COUT/cout for printing of messages to std out
#ifdef PRINTOUT
#define COUT(x) x
#else
#define COUT(x)
#endif


// Use complete log (CLOG/clog) to print messages to std out and the log file
#define clog 
#define CLOG(x) \
COUT(cout x) \
FLOG(tlog x)


// Disabling printing macros 
#define NTLOG(x)
#define NFLOG(x)
#define NCLOG(x)


#endif /* _LOGGER_HH_ */

/*----------------------------------------------------------------------------*/
