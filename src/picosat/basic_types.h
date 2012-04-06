//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        basic_types.h
 *
 * Description: Basic types used by BOLT.
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _BASIC_TYPES_H
#define _BASIC_TYPES_H 1


/*----------------------------------------------------------------------------*\
 * Values besides 0 and 1
\*----------------------------------------------------------------------------*/

#ifdef __LP64__
typedef unsigned long long int ULINT;
typedef long long int LINT;
#define MAXLINT LLONG_MAX;
#define MINLINT LLONG_MIN;
#define MAXULINT ULLONG_MAX;
#else
typedef unsigned long int ULINT;
typedef long int LINT;
#define MAXLINT LONG_MAX;
#define MINLINT LONG_MIN;
#define MAXULINT ULONG_MAX;
#endif

#ifdef GMPDEF
#include <gmpxx.h>
typedef mpz_class XLINT;
#define ToLint(x) x.get_si()
#else
typedef LINT XLINT;
#define ToLint(x) (LINT)x
#endif

#endif /* _BASIC_TYPES_H */

/*----------------------------------------------------------------------------*/
