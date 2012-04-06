/*----------------------------------------------------------------------------*\
 * Version: $Id: ptrace.cc 78 2007-07-27 17:22:20Z jpms $
 *
 * Author: jpms
 * 
 * Description: Implementation of proof tracing.
 *
 *                               Copyright (c) 2005-2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#include "ptrace.hh"


ProofTrace::ProofTrace(char* fname)
{
  assert(0);    // If tracing to disk
}

ProofTrace::ProofTrace() :
  cl_deps(), cl_scheds(), visitq(), depcount(),  _clset(NULL),
  lst_traced_cl(NULL), empty_cl(NULL), dep_stack(),
  _curr_traced_cl(NULL), _curr_traced_deps(NULL),
  _curr_ptr(NULL), _end_ptr(NULL), _curr_lit(-1), _curr_cl(NULL),
  _fname(NULL), _ptmode(false), _num_traced(0) { }

ProofTrace::~ProofTrace()
{
  assert(dep_stack.size() == 0);
  cleanup_proof_trace();
}

void ProofTrace::compute_dep_counts()
{
  assert(lst_traced_cl != NULL);// Start reverse traversal from last learnt cl
  empty_cl = lst_traced_cl;     // Empty clause is the last traced clause
  cl_scheds.insert(empty_cl);
  visitq.push_back(empty_cl);
  depcount[empty_cl] = 1;

  while (!cl_scheds.empty()) {
    _curr_traced_cl = visitq.front();
    visitq.pop_front();
    cl_scheds.erase(_curr_traced_cl);

    ClClDepsMap::iterator clpos = cl_deps.find(_curr_traced_cl);
    assert(clpos != cl_deps.end());
    _curr_traced_deps = clpos->second;

    _curr_ptr = _curr_traced_deps+1;
    _end_ptr =
	_curr_traced_deps + _curr_traced_deps->lit;    // First lit stores size
    while (_curr_ptr != _end_ptr) {
      ClauseRef clref = _curr_ptr->cl;
      if (cl_deps.find(clref) != cl_deps.end()) {      // Learnt clause ...
	//cl_scheds.find(clref) == cl_scheds.end()) {
	map<ClauseRef,int>::iterator cpos;
	if ((cpos = depcount.find(clref)) == depcount.end()) {
	  visitq.push_back(clref);
	  cl_scheds.insert(clref);
	  depcount[clref] = 1;
	} else {
	  depcount[clref]++;
	}
      }
      _curr_ptr++;
    }
  }
  _curr_traced_cl = NULL; _curr_traced_deps = NULL;
  _curr_ptr = NULL; _end_ptr = NULL;
}


void ProofTrace::cleanup_proof_trace()
{
  ClClDepsMap::iterator tpos = cl_deps.begin();
  ClClDepsMap::iterator tend = cl_deps.end();
  for(; tpos!=tend; ++tpos) {
    delete[] tpos->second;
  }
  cl_deps.clear();
  assert(visitq.empty());
  assert(cl_scheds.empty());
}

namespace PTdump {
  void print_dep(ClDepPair* deps) {
    ClDepPair* cptr = deps+1;
    ClDepPair* eptr = deps + deps->lit;
    ClauseRef clref = deps->cl;
    cout << (void*)clref << ": ";
    for(; cptr < eptr; ++cptr) {
      cout << (void*)cptr->cl << " ";
      cout << cptr->lit << " ";
    }
    cout << endl;
  }
};

void ProofTrace::dump(ostream& outs)
{
  // No order... To be defined
  ClClDepsMap::iterator tpos = cl_deps.begin();
  ClClDepsMap::iterator tend = cl_deps.end();
  for(; tpos!=tend; ++tpos) {
    PTdump::print_dep(tpos->second);
  }
  //for_each(cl_deps.begin(), cl_deps.end(), PTdump::print_dep);
}

/*----------------------------------------------------------------------------*/
