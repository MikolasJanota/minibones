/* 
 * File:   minisat_aux.hh
 * Author: mikolas
 *
 * Created on October 21, 2010, 2:10 PM
 */

#ifndef MINISAT_AUX_HH
#define	MINISAT_AUX_HH
#include "core/Solver.h"
#include "auxiliary.hh"
#include <ostream>
using std::ostream;
using std::vector;
using Minisat::Solver;
using Minisat::lbool;
using Minisat::mkLit;
using Minisat::sign;
using Minisat::var;
using Minisat::vec;
using Minisat::Lit;
using Minisat::Var;
typedef std::vector<Lit> LiteralVector;

inline bool is_true(Lit l, const vec<lbool>& model) { 
  const Var v = var(l);
  assert (v<model.size());
  return (model[v]==l_False)==sign(l);
}

ostream& print_model(ostream& out, const vec<lbool>& lv);
void     print(const vec<Lit>& lv);
ostream& print(const Lit& l);
ostream& operator << (ostream& outs, Lit lit);

ostream& print(ostream& out, const vec<Lit>& lv);
inline ostream& operator << (ostream& out, const vec<Lit>& lv) { return print(out,lv); }

/** print values of variables from the given set */
void print(const vector<LINT>& vs);

inline size_t literal_index(Lit l) { 
  assert(var(l) > 0);
  const size_t v = (size_t) var(l);
  return sign(l) ? v<<1 : ((v<<1)+1);
}

inline Lit index2literal(size_t l) { 
  const bool positive = (l&1);
  const Var  variable = l>>1;
  return positive ? mkLit(variable) : ~mkLit(variable);
}
#endif	/* MINISAT_AUX_HH */

