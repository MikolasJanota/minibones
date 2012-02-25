/* 
 * File:   MiniSatExt.hh
 * Author: mikolas
 *
 * Created on November 29, 2010, 5:40 PM
 */

#ifndef MINISATEXT_HH
#define	MINISATEXT_HH
#include "core/Solver.h"

namespace Minisat {
  class MiniSatExt : public Solver {
  public:
    inline void bump(const Var var)        { varBumpActivity(var); }
  };
}
#endif	/* MINISATEXT_HH */

