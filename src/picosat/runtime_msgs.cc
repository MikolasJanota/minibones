//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        runtime_msgs.cc
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

#include "globals.hh"
#include "runtime_msgs.hh"

string RT::tname = "";

string RT::stname = "";


string XRT::get_tname() { return RT::tname; }

void XRT::set_tname(string tn) { RT::tname = tn; }

string XRT::get_stname() { return RT::stname; }

void XRT::set_stname(string nst) { RT::stname = nst; }

/*----------------------------------------------------------------------------*/
