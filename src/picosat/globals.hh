/*----------------------------------------------------------------------------*\
 * Version: $Id: globals.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Include file that includes all other key include files.
 *              This is the include file to be included by the header
 *              files of libraries and tools.
 *
 *                               Copyright (c) 2005-2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _GLOBALS_HH_
#define _GLOBALS_HH_ 1

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <climits>

using namespace std;


/*----------------------------------------------------------------------------*\
 * System configuration
\*----------------------------------------------------------------------------*/

#include "config.hh"


/*----------------------------------------------------------------------------*\
 * Macro definition
\*----------------------------------------------------------------------------*/

#include "macros.hh"


/*----------------------------------------------------------------------------*\
 * Utils for printing debug information
\*----------------------------------------------------------------------------*/

#include "dbg_prt.hh"


/*----------------------------------------------------------------------------*\
 * Definition of types used throughout
\*----------------------------------------------------------------------------*/

#include "basic_types.h"
#include "types.hh"


/*----------------------------------------------------------------------------*\
 * Definition of functors used throughout
\*----------------------------------------------------------------------------*/

#include "functors.hh"


/*----------------------------------------------------------------------------*\
 * Utils for terminating program execution
\*----------------------------------------------------------------------------*/

#include "err_utils.hh"


/*----------------------------------------------------------------------------*\
 * Utils for printing resourse usage information
\*----------------------------------------------------------------------------*/

#include "rusage.hh"


#endif /* _GLOBALS_HH_ */

/*----------------------------------------------------------------------------*/
