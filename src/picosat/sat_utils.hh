//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        sat_utils.hh
 *
 * Description: SAT-related utilities, e.g. printing computed models.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _SAT_UTILS_H
#define _SAT_UTILS_H 1

namespace SATUtils {

  template <typename S, typename T, typename U, typename V>
  void print_model(S& outs, T& model, U topvar, V msg) {
    outs << msg;
    typename T::iterator mpos = model.begin(); 
    typename T::iterator mend = model.end(); if (mpos != mend) { mpos++; }
    for(unsigned long int idx = 1; mpos != mend; ++mpos, ++idx) {
      if (idx > topvar) { break; }
      long int vid = idx;
      if (*mpos > 0)      { outs << vid << " "; }
      else if (*mpos < 0) { outs << -vid << " "; }
      else                { outs << vid << "=0 "; }
    }
    outs << endl;
  }

  template <typename T, typename U, typename V>
  void print_model(T& model, U topvar, V msg) {
    SATUtils::print_model(cout, model, topvar, msg);
  }

}

#endif /* _SAT_UTILS_H */

/*----------------------------------------------------------------------------*/
