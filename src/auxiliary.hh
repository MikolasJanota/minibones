/* 
 * File:   auxiliary.hh
 * Author: mikolas
 *
 * Created on October 20, 2010, 12:32 PM
 */

#ifndef AUXILIARY_HH
#define	AUXILIARY_HH
#include <sys/time.h>
#include <sys/resource.h>
#include <vector>
#include <iostream>
using std::cerr;
using std::endl;

#define __PL (cerr << __FILE__ << ":" << __LINE__ << endl).flush();
#define CONSTANT const
#define CONTAINS(s,e) ( ((s).find(e)) != ((s).end()) )

typedef unsigned int UINT;
typedef long int LINT;
typedef unsigned long int ULINT;

#define FOR_EACH(index,iterated)\
  for (auto index = (iterated).begin(); index != (iterated).end();++index)

class IntHash {
public:
  inline LINT operator()(LINT val) const { return val; }
};

class IntEqual {
public:
  inline bool operator()(LINT v1, LINT v2) const { return v1 == v2; }
};

static inline double read_cpu_time() {
  struct rusage ru; 
  getrusage(RUSAGE_SELF, &ru);
  return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000;
}
#endif	/* AUXILIARY_HH */

