//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        err_utils.hh
 *
 * Description: Utilities to handle errors.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _ERR_UTILS_H
#define _ERR_UTILS_H 1

#include <iostream>

using namespace std;

template <class S>
void tool_abort(S msg) {
  std::cout << "ERROR: " << msg << std::endl; std::cout.flush(); exit(5); }

template <class S>
void tool_warn(S msg) {
  std::cout << "WARN: " << msg << std::endl; std::cout.flush(); }

template <class S>
void tool_info(S msg) {
  std::cout << "INFO: " << msg << std::endl; std::cout.flush(); }

#endif /* _ERR_UTILS_H */

/*----------------------------------------------------------------------------*/
