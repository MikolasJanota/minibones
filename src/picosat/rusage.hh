/*----------------------------------------------------------------------------*\
 * Version: $Id: rusage.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Obtain time and memory resource usage stats.
 *              Adapted from minisat and from MCSAT.
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _RUSAGE_HH_
#define _RUSAGE_HH_ 1

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
using  std::ostream;
using  std::endl;

namespace RUSAGE {
  static inline double read_cpu_time();
  static inline long read_mem_stats(int fields);
  static inline double read_mem_used();
  static inline void print_cpu_time(const char* msg, ostream& outs=std::cout);
  static inline void print_mem_used(const char* msg, ostream& outs=std::cout);
}


static inline double RUSAGE::read_cpu_time()
{
  struct rusage ru; getrusage(RUSAGE_SELF, &ru);
  return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000;
}

#define NSTRSZ 256
static inline long RUSAGE::read_mem_stats(int fields)
{
  if (fields < 0)
    return 0;
  char    name[NSTRSZ];
  pid_t pid = getpid();
  sprintf(name, "/proc/%d/statm", pid);
  FILE*   finp = fopen(name, "rb");
  if (finp == NULL) { /* assert(0); */ return 0; }
  int value;
  for (; fields >= 0; fields--) { fscanf(finp, "%d", &value); }
  fclose(finp);
  return value;
}

#define __1MBYTE__ 1048576

static inline double RUSAGE::read_mem_used()
{
  return ((long)read_mem_stats(0) * (long)getpagesize() * 1.0) / __1MBYTE__;
}

static inline void RUSAGE::print_cpu_time(const char* msg, ostream& outs)
{
  outs << msg << ": "<< RUSAGE::read_cpu_time() << endl;
}

static inline void RUSAGE::print_mem_used(const char* msg, ostream& outs)
{
  outs << msg << ": " << RUSAGE::read_mem_used() << endl;
}

#endif /* _RUSAGE_HH_ */

/*----------------------------------------------------------------------------*/
