//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        runtime_msgs.hh
 *
 * Description: Run time messages. For errors, execution is terminated.
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _RUNTIME_MSGS_H
#define _RUNTIME_MSGS_H 1

#include <iostream>
#include <cstring>


using namespace std;

namespace XRT {
  string get_tname();
  string get_stname();
  void set_tname(string tn);
  void set_stname(string nst);
};

class RT {
  friend string XRT::get_tname();
  friend string XRT::get_stname();
  friend void XRT::set_tname(string tn);
  friend void XRT::set_stname(string nst);
public:
  static string tname;
  static string stname;
};

using namespace XRT;

template <class M>
void Info(M msg)
{
  cout << get_stname() << " INFO: "<< msg << endl; return;
}

template <class M>
void Warn(M msg)
{
  cout << get_stname() << " WARN: "<< msg << endl; return;
}

template <class M>
void Error(M msg)
{
  cout << get_stname() << " ERROR: "<< msg << endl; return;
}

template <class M>
void Abort(M msg) { Error(msg); exit(3); }

#endif /* _RUNTIME_MSGS_H */

/*----------------------------------------------------------------------------*/
