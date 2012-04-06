//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        basic_clset.hh
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

#ifndef _BASIC_CLSET_H
#define _BASIC_CLSET_H 1

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

#define SM_CL_SZ 3


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: BasicClauseSet
 *
 * Purpose: Container for set of clauses.
\*----------------------------------------------------------------------------*/
//jpms:ec

class BasicClauseSet {

public:
  BasicClauseSet() :
    clreg(ClauseRegistry::instance()), clauses(), g2cv_map(), clvect() {
    top_weight = LONG_MAX; def_clw = top_weight; def_cltype = CL_HARD;
    num_hard_cls = num_soft_cls = num_other_cls = num_weighted_cls = 0;
    soft_units = false;
  }

  virtual ~BasicClauseSet() {
    /* Clear actual clauses ... */
    clauses.clear();
    // ANTON: fixed memory leak - vectors need to be deleted
    for (XLInt2ClVMap::iterator pm = g2cv_map.begin();
        pm != g2cv_map.end(); ++pm) {
      if (pm->second != NULL)
        delete pm->second;
    }
    g2cv_map.clear();
    clvect.clear();
  }

  BasicClause* create_clause(LINT nlits, const LINT lits[]) {
    BasicClause* ncl = clreg.create_clause(nlits, lits);
    return insert_clause(ncl);
  }

  BasicClause* create_clause(vector<LINT>& clits) {
    BasicClause* ncl = clreg.create_clause(clits);
    return insert_clause(ncl);
  }

  BasicClause* create_clause(vector<LINT>& clits, LINT clid) {
    BasicClause* ncl = clreg.create_clause(clits);
    set_cl_id(ncl, clid);
    return insert_clause(ncl);
  }

  BasicClause* create_unit_clause(LINT lit) {
    assert(lit != 0);
    LINT lits[] = { lit };
    return create_clause(1, lits);
  }

  BasicClause* create_binary_clause(LINT lit1, LINT lit2) {
    assert(lit1 != 0 && lit2 != 0);
    LINT lits[] = { lit1, lit2 };
    return create_clause(2, lits);
  }

  BasicClause* create_ternary_clause(LINT lit1, LINT lit2, LINT lit3) {
    assert(lit1 != 0 && lit2 != 0 && lit3 != 0);
    LINT lits[] = { lit1, lit2, lit3 };
    return create_clause(3, lits);
  }

  BasicClause* insert_clause(BasicClause* ncl) {
    // Clause is assumed to already exist; no need to set its type
    HashedClauseSet::iterator cpos = clauses.find(ncl);
    if (cpos == clauses.end()) {
      clauses.insert(ncl);
      clreg.incr_cl_refs(ncl);
      set_def_cltype(ncl);    // Clause is new; set type as default type
    } DBG(else { cout << "Clause already exists: "<<*ncl<<endl;});
    return ncl;
  }

  void add_literal(BasicClause* cl, LINT nlit) { clreg.add_literal(cl, nlit); }

  void rm_clause(BasicClause* cl) {
    NDBG(cout << "Removing clause from clset: " << *cl << endl;);
    assert(clauses.find(cl) != clauses.end());
    clauses.erase(cl);
    if (clreg.decr_cl_refs(cl) == 0) { clreg.erase_clause(cl); }
  }

  void erase_clause(BasicClause* cl) {
    assert(clreg.num_cl_refs(cl) == 1);
    rm_clause(cl);  //clreg.erase_clause(cl); // jpms: can only be deleted above
  }

  BasicClause* attach_clause(BasicClause* ncl) {
    // Just add clause to clset
    clauses.insert(ncl);
    return ncl;
  }

  void detach_clause(BasicClause* cl) {
    // Just rm clause from clset
    assert(clauses.find(cl) != clauses.end());
    clauses.erase(cl);  // Clause is *not* deleted; This is fragile ...
  }

  BasicClause* tmp_add_clause(BasicClause* ncl) {
    // Temporary clause addition. Clause must have only been *partially* removed
    return attach_clause(ncl);    // Clause counter is unchanged
  }

  void tmp_rm_clause(BasicClause* cl) {
    // Temporary clause removal. Clause is only *partially* removed
    NDBG(cout << "Extracting clause from clset: " << *cl << endl;);
    detach_clause(cl);
  }

  BasicClause* lookup(vector<LINT>& clits) { return clreg.lookup_vect(clits); }

  BasicClause* lookup(LINT num, const LINT ivect[]) {
    return clreg.lookup_vect(num, ivect);
  }

  ULINT get_cl_min_lit(BasicClause* cl) { return cl->get_min_lit(); }

  ULINT get_cl_max_lit(BasicClause* cl) { return cl->get_max_lit(); }

  void set_num_vars(ULINT nvars) { }

  ULINT get_num_vars() { return -1; }

  void set_num_cls(ULINT ncls) { }

  ULINT get_num_cls() { return size(); }

  void set_top(XLINT topv) { assert(clauses.size() == 0); top_weight = topv; }

  XLINT get_top() { return top_weight; }

  void set_num_grp(XLINT ngrp) { top_weight = ngrp; }

  XLINT get_num_grp() { return top_weight; }

  vector<LINT>& get_cl_lits(BasicClause* cl) { return cl->cl_lits(); }

  void cl_lits(BasicClause* cl, IntVector lvect) {
    Literator lpos = cl->begin(); Literator lend = cl->end();
    for(; lpos != lend; ++lpos) { lvect.push_back(*lpos); }
  }

  // Preferably use incr_cl_weight
  void set_cl_weight(BasicClause* cl, XLINT clweight = 1) {
    // For repeated clauses, need to update existing weight
    cl->set_weight((clweight >= top_weight) ? top_weight : clweight);
    // Stats
    if (clweight == top_weight) { ++num_hard_cls; }
    else if (clweight == 1)     { ++num_soft_cls; }
    else                        { ++num_other_cls; }
    ++num_weighted_cls;
  }

  XLINT get_cl_weight(BasicClause* cl) { return cl->get_weight(); }

  XLINT incr_cl_weight(BasicClause* cl, XLINT incr) {
    // For repeated clauses, need to update existing weight
    if (!is_cl_hard(cl)) {
      XLINT nw = (cl->get_weight() + incr < top_weight) ?
	(cl->get_weight() + incr) : top_weight;
      cl->set_weight(nw);
    } //else { assert(0); }
    return cl->get_weight();
  }

  XLINT decr_cl_weight(BasicClause* cl, XLINT decr) {
    // For repeated clauses, need to update existing weight
    if (!is_cl_hard(cl)) { cl->set_weight(cl->get_weight() - decr); }
    else                 { assert(0); }
    return cl->get_weight();
  }

  void set_cl_id(BasicClause* cl, ULINT clid) { cl->set_id(clid); }

  ULINT get_cl_id(BasicClause* cl) { return cl->get_id(); }

  void set_cl_grp_id(BasicClause* cl, ULINT gid) { cl->set_grp_id(gid); }

  ULINT get_cl__grp_id(BasicClause* cl) { return cl->get_grp_id(); }

  void set_cl_group(BasicClause* cl, XLINT clgrp) {
    cl->set_weight(clgrp);
    vector<BasicClause*>* bcvect = NULL;
    XLInt2ClVMap::iterator ipos = g2cv_map.find(clgrp);
    if (ipos == g2cv_map.end()) {
      bcvect = new vector<BasicClause*>();
      g2cv_map.insert(make_pair(clgrp, bcvect));
    }
    else {
       bcvect = ipos->second;
       bcvect->push_back(cl);
    }
  }

  XLINT get_cl_group(BasicClause* cl) {
    return cl->get_weight();
  }

  void set_cl_hard(BasicClause* cl) { set_cl_weight(cl, top_weight); }

  void set_cl_soft(BasicClause* cl) { set_cl_weight(cl, 1); }

  bool is_cl_hard(BasicClause* cl) {
    XLINT clw = get_cl_weight(cl);
    return clw >= top_weight;
  }

  bool is_cl_soft(BasicClause* cl) {
    XLINT clw = get_cl_weight(cl);
    return clw < top_weight;
  }

  bool is_cl_unit(BasicClause* cl) {  return cl->size() == 1; }

  void set_def_cl_hard() { def_cltype = CL_HARD; }

  void set_def_cl_soft() { def_cltype = CL_SOFT; }

  void set_def_cl_weight(XLINT nclw) {
    def_cltype = CL_WEIGHTED; def_clw = nclw; }

public:    // Properties of clause set: hard & soft clauses

  bool all_soft_unit() { return soft_units; }

  void compute_properties() {
    compute_hard_cl_properties(); compute_soft_cl_properties(); }

  void compute_hard_cl_properties() { }

  void compute_soft_cl_properties() {
    DBG(LINT clcnt = size(); LINT scnt = 0;);
    cset_iterator cpos = begin();
    cset_iterator cend = end();
    soft_units = true;
    for(; cpos!=cend; ++cpos) {
      BasicClause* cl = *cpos;
      if (is_cl_soft(cl)) {
	if (cl->size() != 1) { soft_units = false; break; }
	DBG(scnt++; if (cl->size() != 1) { soft_units = false; });
      }
    }
    DBG(cout << "Number of clauses: " << clcnt << endl;
	cout << "  Soft clauses:    " << scnt << endl;
	cout << "All soft clauses are unit.\n";);
  }

public:    // Access to clauses

  cset_iterator begin() { return clauses.begin(); }

  cset_iterator end() { return clauses.end(); }

  cset_iterator find(BasicClause* cl) { return clauses.find(cl); }

  ULINT size() { return clauses.size(); }

  ClVectIterator svect_begin() {
    // Resize vector of cl pointers
    if (clvect.size() < clauses.size()) { clvect.resize(clauses.size()); }
    // Fill in vector of cl pointers
    ClSetIterator cpos = clauses.begin();
    ClSetIterator cend = clauses.end();
    ClVectIterator vpos = clvect.begin();
    for (; cpos != cend; ++cpos, ++vpos) { *vpos = *cpos; }
    // Sort cl pointers
    sort(clvect.begin(), clvect.end(), PtrLess());
    // Return iterator for sorted vector of cl pointers
    return clvect.begin();
  }

  ClVectIterator svect_end() { return clvect.end(); }

public:    // Clean up

  inline void clear() {
#if USE_CLAUSE_HASH
    // when defined, clause content is used to calculate the hashes; if this
    // is the case, clear_direct() cannot be used, because iteration over hash
    // set uses the hash values, and when the clause is deleted the hashvalue
    // is not available
    clear_indirect();
#endif
    clear_direct();
  }

  // Simple clear, with a single pass, currently being tested (jpms@20100415)
  inline void clear_direct() {
    cset_iterator cpos = begin();
    cset_iterator cend = end();
    for(LINT i=0; cpos != cend; ++cpos, ++i) {
      BasicClause* cl = *cpos;
      if (clreg.decr_cl_refs(cl) == 0) { clreg.erase_clause(cl); }
    }
    clauses.clear();
  }

  // Safe version of clear, using vector
  inline void clear_indirect() {
    BasicClauseVector clvect; clvect.resize(size(), NULL);
    cset_iterator cpos = begin();
    cset_iterator cend = end();
    for(LINT i=0; cpos != cend; ++cpos, ++i) { clvect[i] = *cpos; }
    clauses.clear();
    BasicClauseVector::iterator dpos = clvect.begin();
    BasicClauseVector::iterator dend = clvect.end();
    for(; dpos != dend; ++dpos) {
      BasicClause* cl = *dpos;
      if (clreg.decr_cl_refs(cl) == 0) { clreg.erase_clause(cl); }
    }
    clvect.clear();
  }

public:    // Stats

  ULINT get_num_hard_cls() { return num_hard_cls; }

  ULINT get_num_soft_cls() { return num_soft_cls; }

  ULINT get_num_nonsofthard_cls() { return num_other_cls; }

  ULINT get_num_weighted_cls() { return num_weighted_cls; }

public:    // Output functions

  void dump(ostream& outs=cout) {
    DBG(LINT clcnt = 0);
    cset_iterator cpos = clauses.begin();
    cset_iterator cend = clauses.end();
    for (; cpos != cend; ++cpos) {
      outs << **cpos << " ";
      outs << endl;
      DBG(clcnt++;)
    }
    DBG(outs<<"Done... CLSET Size: "<<clcnt<<endl;);
    //outs << endl;
  }

  friend ostream & operator << (ostream& outs, BasicClauseSet& cl) {
    cl.dump(outs);
    return outs;
  }

protected:

  inline void set_def_cltype(BasicClause* cl) {
    switch (def_cltype) {
    case CL_HARD:
      set_cl_hard(cl);
      break;
    case CL_SOFT:
      set_cl_soft(cl);
      break;
    case CL_WEIGHTED:
      set_cl_weight(cl, def_clw);
      break;
    }
  }

protected:

  ClauseRegistry& clreg;

  HashedClauseSet clauses;

  XLInt2ClVMap g2cv_map;

  XLINT top_weight;

  ClauseTypes def_cltype;

  XLINT def_clw;

  ULINT num_hard_cls, num_soft_cls, num_other_cls, num_weighted_cls;

  bool soft_units;

  BasicClauseVector clvect;

  bool iteron;

private:

  IntVector wrk_lits;

};

#endif /* _BASIC_CLSET_H */

/*----------------------------------------------------------------------------*/
