//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        str_utils.hh
 *
 * Description: Generation of strings from integers, floats, doubles, etc.
 *              Inspired on examples from parashift.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _STR_UTILS_H
#define _STR_UTILS_H 1

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
 
class BadConversion : public std::runtime_error {
public:
  BadConversion(const std::string& s) : std::runtime_error(s) { }
};
 
template <class T>
inline std::string stringit(T x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion("stringify(double)");
  return o.str();
}

#endif /* _STR_UTILS_H */

/*----------------------------------------------------------------------------*/
