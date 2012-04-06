//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        cl_globals.hh
 *
 * Description: Complete definitions for clauses and clause sets.
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _CL_GLOBALS_H
#define _CL_GLOBALS_H 1

#include <cmath>
#include <vector>

#include <ext/hash_map>       // Location of STL hash extensions
#include <ext/hash_set>       // Location of STL hash extensions

using namespace std;
using namespace __gnu_cxx;    // Required for STL hash extensions

#include "globals.hh"
#include "basic_clause.hh"
#include "cl_functors.hh"
#include "cl_types.hh"
#include "cl_registry.hh"

#endif /* _CL_GLOBALS_H */

/*----------------------------------------------------------------------------*/
