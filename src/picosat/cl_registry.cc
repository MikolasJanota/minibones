//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        cl_registry.cc
 *
 * Description: Definition of static variables for the ClauseRegistry.
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#include "cl_registry.hh"

ClauseRegistry* ClauseRegistry::clreg_instance = new ClauseRegistry();

BasicClause* ClauseRegistry::lookup_vect(vector<LINT>& clits) {
    assert(is_sorted(clits.begin(), clits.end(), AbsLitLess()));
    iv2cl_iterator ippos = v2p_map.find(&clits);
    return (ippos != v2p_map.end()) ? ippos->second : NULL;
  }


/*----------------------------------------------------------------------------*/
