//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        basic_clause.hh
 *
 * Description: A basic clause is a vector of literals.
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _BASIC_CLAUSE_H
#define _BASIC_CLAUSE_H 1

#include "globals.hh"
#include "cl_id_manager.hh"

// Anton: added a bit of code to support various pre-processing and simplification 
// techniques during MUS extraction. There's basically no run-time overhead 
// except a few initializations (note that most of the methods are left unchanged
// and so should not be used in applications that simplify clauses). The code is
// compiled in only when EXTRAS_ANTON is defined. If you want to use it and have
// any questions, ask me.

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Basic types for class BasicClause.
\*----------------------------------------------------------------------------*/
//jpms:ec

typedef vector<LINT> LitVector;
typedef vector<LINT>::iterator Literator;
typedef vector<LINT>::reverse_iterator RLiterator;
typedef vector<LINT>::const_iterator CLiterator;
typedef ULINT GID;      // group id
const GID gid_Undef = (GID)-1L;

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: BasicClause
 *
 * Purpose: New organization of clauses, to be used in future tools
\*----------------------------------------------------------------------------*/
//jpms:ec

class BasicClause {
  friend class ClauseRegistry;
  friend class BasicClauseSet;
  friend class BasicGroupSet;
protected:

  BasicClause(const vector<LINT>& lits) 
    : clits(lits), weight(0), id(ClauseIdManager::Instance()->new_id()), 
      grp_id(gid_Undef)
#ifdef EXTRAS_ANTON
    , _aend(clits.end()), _asize(clits.size()), _abstr(calculate_abstr()),
      _nv_count(0), _g0v_count(0), _ss_id(0)
#endif
  {
    assert(adjacent_find(lits.begin(),lits.end(),AbsLitGreater())==lits.end());
    //sort_lits();
    NDBG(cout << "Creating clause: [" << *this << "]" << endl;);
  }

  virtual ~BasicClause(void) { clits.clear(); }

public:

  ULINT size() const { return clits.size(); }

  Literator begin() { return clits.begin(); }
  CLiterator begin() const { return clits.begin(); }

  Literator end() { return clits.end(); }
  CLiterator end() const { return clits.end(); }

protected:

  void add_lit(LINT lit) {
    clits.push_back(lit);    // Currently sorts; it is simpler to insert & shift
    assert(clits.size() > 0);
    if (abs(lit) < abs(clits[clits.size()-1])) {
      sort_lits();    // alternatively: 
      /*  => Seems to be less efficient
	  Literator pos2 = clits.end();
	  Literator pos1 = clits.end();
	  --pos1;
	  for (--pos1; abs(*pos1) > abs(lit); --pos1, --pos2) {
	  *pos2 = *pos1;
	  if (pos1 == clits.begin()) { break; }
	  }
	  *pos1 = lit;
	  */
      //update_internal_data((unsigned)lit);
    }
  }

  void del_lit(LINT lit) {
    for (Literator pos = clits.begin(); pos != clits.end(); ++pos) {
      if (*pos == lit) { *pos = clits.back(); break; }
    }
    clits.pop_back();    // Currently sorts; it is simpler to insert & shift
    //sort_lits();  // alternatively: 
    //update_internal_data((unsigned)lit);   // hash XOR removes bits due to lit
  }

  vector<LINT>& cl_lits() { return clits; }

  //ULINT clhash() { return hashval; }

  ULINT get_min_lit() { ULINT minid=abs(clits[0]); return minid; }

  ULINT get_max_lit() { ULINT maxid=abs(clits[clits.size()-1]); return maxid; }

public:    // Weight/group/id functions

  void set_weight(XLINT nweight) { weight = nweight; }

  XLINT get_weight() const { return weight; }

  void set_grp_id(GID grp) { grp_id = grp; }

  GID get_grp_id() const { return grp_id; }

  void set_id(ULINT nid) { id = nid; }

  ULINT get_id() const { return id; }

public:    // Comparison functions

  friend bool operator > (const BasicClause& ptr1, const BasicClause& ptr2) {
    return
      (ptr1.size() > ptr2.size()) ||
      (ptr1.size() == ptr2.size() && ptr1.get_id() > ptr2.get_id());
  }

  friend bool operator < (const BasicClause& ptr1, const BasicClause& ptr2) {
    return
      (ptr1.size() < ptr2.size()) ||
      (ptr1.size() == ptr2.size() && ptr1.get_id() < ptr2.get_id());
  }

public:    // Output functions

  // debugging version: prints group ID in front of the clause
  void dump(ostream& outs=cout) const {
    outs << "[" << get_grp_id() << "] " << *this;
#ifdef EXTRAS_ANTON
    outs <<  "(r=" << removed() << ", asz=" << asize() << ")";
#endif 
  }

  friend ostream & operator << (ostream& outs, const BasicClause& cl) {
    CLiterator lpos = cl.clits.begin();
    CLiterator lend = cl.clits.end();
    for (; lpos != lend; ++lpos) {
      outs << *lpos << " ";
    }
    outs << "0";
    return outs;
  }

protected:

  void sort_lits() {
    //cout << "Lits A: ";
    //copy(clits.begin(), clits.end(), ostream_iterator<int>(cout, " "));
    //cout << endl;
    //if (!is_sorted(clits.begin(), clits.end(), AbsLitLess())) { }
    sort(clits.begin(), clits.end(), AbsLitLess());
    //cout << "Lits B: ";
    //copy(clits.begin(), clits.end(), ostream_iterator<int>(cout, " "));
    //cout << endl;
  }

  ULINT compute_hash() {    // Not being used: see below new hash functions
    exit(0);
    register ULINT hashv = 0;
    for(vector<LINT>::iterator pos = clits.begin(); pos != clits.end(); ++pos) {
      hashv ^= (*pos>0) ? *pos : -*pos;
    }
    return hashv;
  }

  //void update_internal_data(ULINT uval) { sort_lits(); hashval ^= uval; }

protected:

  vector<LINT> clits;

  //ULINT hashval;

  XLINT weight;

  ULINT id;

  GID grp_id; // ANTON: the actual group ID, rather than using id

#ifdef EXTRAS_ANTON

  /* This part is added to support various pre-processing and simplification 
   * techniques during MUS extraction. See comments at the top of this file.
   */

public: 

  /** Clause removal */
  void mark_removed(void) { _flags.removed = 1; }
  void unmark_removed(void) { _flags.removed = 0; }
  bool removed(void) const { return _flags.removed; }

  /** Clauses might become unsorted during some operations */
  void mark_sorted(void) { _flags.unsorted = 0; }
  void mark_unsorted(void) { _flags.unsorted = 1; }
  bool unsorted(void) const { return _flags.unsorted; }
  // sorts active literals and marks the clause not un-sorted
  void sort_alits(void) { 
    sort(clits.begin(), _aend, AbsLitLess()); 
    _flags.unsorted = 0;
  }

  /** Iterators over active literals -- i.e. those that are non-false.
   */
  ULINT asize(void) const { return _asize; }
  Literator abegin(void) { return clits.begin(); }
  CLiterator abegin(void) const { return clits.begin(); }
  Literator aend(void) { return _aend; }
  CLiterator aend(void) const { return _aend; }

  /** Direct access to literals */
  vector<LINT>& lits(void) { return clits; }

  /** Shrinks active size by 1 */
  void shrink(void) { _asize--; _aend--; update_abstr(); }

  /** Restores active size to the original size */
  void restore(void) { _asize = size(); _aend = clits.end(); update_abstr(); }

  /** Returns iterator to a literal of variable v in the active part
   * or the clause, or aend() if not found. 
   */
  CLiterator afind(ULINT var) const
  {
    CLiterator res = abegin();
    if (unsorted())
      while (res != aend() && (ULINT)abs(*res) != var) ++res;
    else {
      res = lower_bound(res, aend(), var, AbsLitLess()); // binary search
      if (res != aend() && (ULINT)abs(*res) != var)
        res = aend();
    }
    return res;
  }

  /** Returns the abstraction of the clause */
  ULINT abstr(void) const { return _abstr; }

  /* Updates abstraction -- call when clause changes */
  void update_abstr(void) { _abstr = calculate_abstr(); }

  /** Access to the counter of necessary variables */
  const unsigned& nv_count(void) const { return _nv_count; }
  unsigned& nv_count(void) { return _nv_count; }

  /** Access to the counter of group 0 variables */
  const unsigned& g0v_count(void) const { return _g0v_count; }
  unsigned& g0v_count(void) { return _g0v_count; }

  /** Access to the solver-ID of this clause */
  const unsigned& ss_id(void) const { return _ss_id; }
  unsigned& ss_id(void) { return _ss_id; }

  /** Writes out the active part of the clause */
  std::ostream& awrite(std::ostream& outs = cout) const {
    for (CLiterator lpos = abegin(); lpos != aend(); ++lpos)
      outs << *lpos << " ";
    outs << "0";
    return outs;
  }

protected:

  /* Calculates abstraction of the active part of clause (from scratch) */
  ULINT calculate_abstr(void) {
    ULINT abstr = 0;
    const ULINT sz = 8*sizeof(ULINT) - 1;
    for (CLiterator plit = abegin(); plit != aend(); ++plit) {
      ULINT p = (((ULINT)abs(*plit) - 1) << 1) | (*plit < 0);
      abstr |= (ULINT)1 << ((p ^ (p >> 3)) & sz);
    }
    return abstr;
  }
  
  // various useful flags
  struct flags {
    unsigned removed   : 1;     // 1 when the clause is removed (e.g. BCE or sat)
    unsigned unsorted  : 1;     // 1 when the clause might be un-sorted
    unsigned unused    : 30; 
    flags(void) { removed = 0; unsorted = 0; }
  } _flags;
  
  // all falsified literals will be moved to the end of the clause; note that
  // since all literals are sorted, the remaining literals will also be sorted;
  // thus, all we need to do is to keep track of the "real" end of the clause.

  Literator _aend;       // initially lits.end()
  
  size_t _asize;         // initially lits.size()

  ULINT _abstr;          // abstraction

  unsigned _nv_count;    // count of necessary variables (needed for VMUS)

  unsigned _g0v_count;   // count of group 0 variables (needed for VMUS)

  unsigned _ss_id;       // solver-ID of this clause

#endif

};

#endif /* _BASIC_CLAUSE_H */

/*----------------------------------------------------------------------------*/
