/*----------------------------------------------------------------------------*\
 * Version: $Id: dbg_prt.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Utilities for printing containers.
 *
 *                               Copyright (c) 2005-2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _DBG_PRT_HH_
#define _DBG_PRT_HH_ 1

#include <iostream>

template <class T>
inline void PRINT_ELEMENTS (const T& coll,
			    const char* optcstr="", const char* sepstr=" ")
{
  typename T::const_iterator pos;
  std::cout << optcstr;

  for(pos=coll.begin(); pos!=coll.end(); ++pos) {
    std::cout << *pos << sepstr;
  }
  std::cout << std::endl;
}

template <class T>
inline void PACK_PRINT_ELEMENTS (const T& coll, const char* optcstr="")
{
  typename T::const_iterator pos;
  std::cout << optcstr;

  for(pos=coll.begin(); pos!=coll.end(); ++pos) {
    std::cout << *pos;    //((bool)*pos)?1:0;  // -> This only works for bool's
  }
  std::cout << std::endl;
}

template <class T>
inline void PRINT_PTR_ELEMENTS (const T& coll,
				const char* optcstr="", const char* sepstr=" ")
{
  typename T::const_iterator pos;
  std::cout << optcstr;

  for(pos=coll.begin(); pos!=coll.end(); ++pos) {
    std::cout << **pos << sepstr;
  }
  std::cout << std::endl;
}

#endif /* _DBG_PRT_HH_ */

/*----------------------------------------------------------------------------*/
