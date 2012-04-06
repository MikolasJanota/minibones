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

/*----------------------------------------------------------------------------*/
