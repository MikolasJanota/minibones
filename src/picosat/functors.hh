//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        functors.hh
 *
 * Description: 
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _FUNCTORS_H
#define _FUNCTORS_H 1

template<class T> struct defprint : public unary_function<T, void>
{
  defprint(ostream& out) : os(out) {}
  void operator() (T x) { os << x << ' '; }
  ostream& os;
};

template<class T> struct ptrprint : public unary_function<T, void>
{
  ptrprint(ostream& out) : os(out) {}
  void operator() (T x) { os << *x << endl; }
  ostream& os;
};


#endif /* _FUNCTORS_H */

/*----------------------------------------------------------------------------*/
