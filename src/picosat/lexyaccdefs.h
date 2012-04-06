
#include <assert.h>

#ifndef DBG
#ifdef FULLDEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif
#endif
#define DBGNO(x)
#define NDBG(x) DBGNO(x)
