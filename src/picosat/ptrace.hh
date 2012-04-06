/*----------------------------------------------------------------------------*\
 * Version: $Id: ptrace.hh 78 2007-07-27 17:22:20Z jpms $
 *
 * Author: jpms
 * 
 * Description: Proof tracing ability. In this version, tracing of for an
 *              internal data structure. Should consider tracing to disk.
 *              Note: format of proof trace is:
 *                    vectsize recclid clid1 -1 clid2 vid1 clid2 vid3 ...
 *
 *                               Copyright (c) 2005-2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _PTRACE_HH_
#define _PTRACE_HH_ 1

#include <iostream>
#include <iomanip>
#include <cassert>

#include <vector>
#include <list>

#include "globals.hh"
#include "proof_logger.hh"

using namespace std;


/*----------------------------------------------------------------------------*\
 * Clause dependency pairs
\*----------------------------------------------------------------------------*/

class ClDepPair {
public:
  ClDepPair() : cl(NULL), lit(-1) { }
  ClDepPair(ClauseRef ncl, int nlit) : cl(ncl), lit(nlit) { }
  ClauseRef cl;
  int lit;
};


/*----------------------------------------------------------------------------*\
 * Typedefs
\*----------------------------------------------------------------------------*/

// Maps cl ref's into clause deps
typedef ClRefMap<ClDepPair*> ClClDepsMap;



/*----------------------------------------------------------------------------*\
 * Class: ProofTrace
 *
 * Purpose: Proof tracing / logging mechanism, that enables recovering a
 *          resolution proof
\*----------------------------------------------------------------------------*/

class ProofTrace : public ProofLogger {

public:

  ProofTrace(char* fname);    // If tracing to disk
  ProofTrace();

  virtual ~ProofTrace();

public:    // Functions that build the trace

  inline void begin_cl_trace() {  // Nothig to do, just checks...
    assert(dep_stack.size() == 0);
    assert(_ptmode == false);
  }

  void add_dep_pair(ClauseRef cl, int lit) {
    ClDepPair cp(cl,lit);
    dep_stack.push_back(cp);
  }

  inline void end_cl_trace(ClauseRef clref) {
    ClDepPair* deps = new ClDepPair[dep_stack.size()+1];

    deps[0].cl = clref;                  // Unnecessary, but useful for debug
    deps[0].lit = dep_stack.size()+1;    // Size is kept in the first position
    ClDepPair *px = deps+1;
    while (dep_stack.size()) {
      ClDepPair cp = dep_stack.front();
      px->cl = cp.cl; px->lit = cp.lit;
      ++px;
      dep_stack.pop_front();
    }
    assert(dep_stack.size()==0);
    DBG(cout << "New clause(" << clref << "): "<< *clref;);
    assert(!cl_deps.exists(clref));
    cl_deps.insert(clref, deps);
    lst_traced_cl = clref;
  }

  inline int num_traced_clauses() { return cl_deps.size(); }

public:    // Functions that traverse the trace

  //----------------------------------------------------------------------------
  // Proof trace traversal.
  //----------------------------------------------------------------------------

  // Traversal *must* be from empty clause
  inline void begin_traversal(ClRefSet* clset = NULL) {
    _ptmode = true;
    _clset = clset;    // Internal ptr for clause set (to store core)
    _num_traced = 0;
    assert(lst_traced_cl != NULL);// Start reverse traversal from last learnt cl
    compute_dep_counts();         // Use dep counts to implements a BFS

    empty_cl = lst_traced_cl;     // Empty clause is the last traced clause
    cl_scheds.insert(empty_cl);
    visitq.push_back(empty_cl);
    depcount[empty_cl]--;
    assert(depcount[empty_cl] == 0);
  }

  inline bool has_cl2traverse() {
    assert(!cl_scheds.empty() || visitq.empty());
    return !cl_scheds.empty();
  }

  inline void update_cl2traverse() {
    _curr_traced_cl = visitq.front();
    visitq.pop_front();
    cl_scheds.erase(_curr_traced_cl);
    assert(depcount[_curr_traced_cl] == 0);

    ClClDepsMap::iterator clpos = cl_deps.find(_curr_traced_cl);
    assert(clpos != cl_deps.end());
    _curr_traced_deps = clpos->second;
  }

  ClauseRef curr_traced_clause() { return _curr_traced_cl; }  // Cl being traced

  ClDepPair* curr_traced_cldeps() { return _curr_traced_deps; }

  inline bool curr_is_learnt() {  // If cl is learnt, it has dependencies
    return _curr_traced_deps->lit > 1;
  }

  bool is_learnt(ClauseRef cl) { return cl_deps.exists(cl); }

  inline void next_cl2traverse() {
    // Visit deps, and schedule clauses
    _curr_ptr = _curr_traced_deps+1;
    _end_ptr =
      _curr_traced_deps + _curr_traced_deps->lit;    // First lit stores size
    while (_curr_ptr != _end_ptr) {
      ClauseRef clref = _curr_ptr->cl;
      if (cl_deps.find(clref) != cl_deps.end()) {    // Learnt clause ...
	//cl_scheds.find(clref) == cl_scheds.end() &&
	if (--depcount[clref] == 0) {
	  visitq.push_back(clref); cl_scheds.insert(clref);
	  _num_traced++;
	  DBG(cout<<"Scheduling "<<(void*)clref<<" to trace"<<endl);
	}
      } else if (_clset != NULL && _clset->find(clref) == _clset->end()) {
	_clset->insert(clref);
      }
      _curr_ptr++;
    }
  }

  inline void end_traversal() {
    _ptmode = false;        // Do nothing else for now...
    _clset = NULL;
  }

  inline int num_visited_clauses() { return _num_traced; }

protected:

  void update_traverse_data() { /* To be defined ... */ }

public:    // Functions that traverse clause deps

  //----------------------------------------------------------------------------
  // Clause deps traversal, in reverse order
  //----------------------------------------------------------------------------

  inline void init_trace_cl_deps(ClauseRef cl, ClDepPair* cdeps) {
    assert(_ptmode == false);
    _curr_ptr = cdeps + (cdeps->lit-1); // Size is in lit; -1 to start valid pos
    _end_ptr = cdeps;
    assert((cdeps+1)->lit == 0);        // First dep *must* always be 0
  }

  inline bool has_dep_pair() { return _curr_ptr != _end_ptr; }

  inline void update_dep_pair() {
    assert(_curr_ptr != _end_ptr);
    _curr_lit = _curr_ptr->lit;
    _curr_cl = _curr_ptr->cl;
    DBG(cout<<"cl: "<<(void*)_curr_cl<<" lit: "<<_curr_lit<<endl);
  }

  inline void next_dep_pair() { _curr_ptr--; }

  inline int curr_lit() { return _curr_lit; }      // Lit of current pair

  inline ClauseRef curr_cl() { return _curr_cl; }  // Cl ID of current pair

  inline void end_trace_cl_deps() {
    _curr_cl = NULL; _curr_lit = -1;
    _curr_ptr = _end_ptr = NULL;
  }

public:

  void dump(ostream& outs);

  friend ostream & operator << (ostream& outs, ProofTrace& pt) {
    pt.dump(outs);
    return outs;
  }

protected:

  void cleanup_proof_trace();

  void compute_dep_counts();

protected:

  ClRefMap<ClDepPair*> cl_deps;  // The actual clause dependencies

  ClRefSet cl_scheds;            // Clauses scheduled for visit
  list<ClauseRef> visitq;        // Queue of clauses to visit

  map<ClauseRef,int> depcount;   // Dependency count for each clause

  ClRefSet* _clset;              // Set of clauses in unsat core (optional)

  ClauseRef lst_traced_cl;       // Last traced cl
  ClauseRef empty_cl;            // Empty clause

  list<ClDepPair> dep_stack;     // Current traced deps

  ClauseRef _curr_traced_cl;
  ClDepPair* _curr_traced_deps;

  ClDepPair* _curr_ptr;
  ClDepPair* _end_ptr;
  int _curr_lit;
  ClauseRef _curr_cl;

  char* _fname;                  // Name of file to contain the proof trace

  bool _ptmode;                  // Proof trace mode (f=tracing; t=traversing)

  int _num_traced;               // Number of clauses traced in proof trace

};

#endif /* _PTRACE_HH_ */

/*----------------------------------------------------------------------------*/
