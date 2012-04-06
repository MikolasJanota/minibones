//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_incr_wrapper.cc
 *
 * Description: Incremental interface for running picosat.
 *
 * Author:      jpms
 *
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#include <stdio.h>

#include "globals.hh"
#include "logger.hh"
#include "str_utils.hh"
#include "picosat_incr_wrapper.hh"

#define CORE_THRESHOLD 100

extern "C" {
  extern void picosat_init (void);               /* constructor */
  extern void picosat_reset (void);              /* destructor */

  extern void picosat_enter ();
  extern void picosat_leave ();

  extern void picosat_adjust (int max_idx);

  extern void picosat_enable_trace_generation (void);

  extern int picosat_coreclause(int i);
  extern int picosat_corelit (int lit);
  extern int picosat_failed_assumption (int lit);

  extern void picosat_write_clausal_core (FILE * core_file);

  extern void picosat_assume (int lit);

  extern int picosat_add (int lit);
  extern int picosat_sat (int decision_limit);
  extern int picosat_deref (int lit);
  extern int picosat_inconsistent();

  extern void picosat_set_more_important_lit (int lit);
  extern void picosat_set_less_important_lit (int lit);

  extern void picosat_set_seed (unsigned random_number_generator_seed);
  extern void picosat_set_global_default_phase (int);

  extern void picosat_reset_phases (void);
  extern void picosat_reset_scores (void);
  extern void picosat_remove_learned (unsigned percentage);

  extern void picosat_print (FILE *);
  extern void picosat_stats (void);
  extern void picosat_set_verbosity (int new_verbosity_level);

  extern int picosat_toplevel (void);
  extern int picosat_deref_level (int lit);
  extern int picosat_decision_lit (int lit);

}


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Purpose: Invoke picosat constructor and set up data structures
\*----------------------------------------------------------------------------*/
//jpms:ec

void PicosatIncrementalWrapper::init(bool traceon)
{
  iclset = NULL; iassume = NULL; ret_vect = NULL;
  maxvid = 0;
  picosat_init();

  if (traceon) { picosat_enable_trace_generation(); ccls = true; }
  picosat_set_seed(0);

  //picosat_set_global_default_phase(1);      // Specify var phase
  picosat_set_global_default_phase (phase);
  picosat_set_verbosity (verbosity);
}

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Purpose: Clean up data structures and invoke picosat destructor
\*----------------------------------------------------------------------------*/
//jpms:ec

void PicosatIncrementalWrapper::reset()
{
  picosat_reset();
  iclset = NULL; iassume = NULL; ret_vect = NULL; 
}

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Purpose: Interfaces the picosat solver in incremental mode
 *
 * _iclset: Additional clauses to add to CNF formula
 * _iassume: Assumptions to consider in this call
 * _ret_vect: Place holder for core information or computed model
\*----------------------------------------------------------------------------*/
//jpms:ec

int
PicosatIncrementalWrapper::solve(BasicClauseSet& _iclset, IntVector& _iassume,
				 IntVector& _ret_vect)
{
  assert(iclset == NULL && iassume == NULL && ret_vect == NULL);
  iclset = &_iclset; iassume = &_iassume; ret_vect = &_ret_vect;
  assert(_ret_vect.size() == 0);
  // 1. Load new set of clauses into picosat
  transmit_clauses_set();

  // 2. Load assumptions into picosat
  transmit_assumptions();

  // 3. Invoke SAT solver
  //prt_std_cputime("c ", "Running SAT solver ...");
  // picosat_set_global_default_phase(1);      // Specify var phase
  picosat_set_seed(0);
  int status = picosat_sat(-1);    // No limit on decisions
  DBG(cout<<"Status: "<<status<<endl; cout<<"Max var ID: "<<maxvid<<endl;);
  if (verbosity > 0) { prt_std_cputime("c ", "Done running SAT solver ..."); }

  // 4. Analyze outcome and get relevant data
  if (status == 20) {             // Unsatisfiable
    handle_unsat_outcome(); }
  else if (status == 10) {        // Satisfiable
    handle_sat_outcome(); }
  else { Abort("Invalid SAT solver outcome. Contact author."); }

  // 5. Stats & Cleanup
  //picosat_stats();
  picosat_reset_scores();
  picosat_reset_phases();
  iclset = NULL; iassume = NULL; ret_vect = NULL;
  return status;
}

int
PicosatIncrementalWrapper::solve(BasicClauseVector& _iclvect,
				 IntVector& _iassume,
				 IntVector& _ret_vect)
{
  assert(iclset==NULL && iclvect==NULL && iassume==NULL && ret_vect==NULL);
  iclvect = &_iclvect; iassume = &_iassume; ret_vect = &_ret_vect;
  // 1. Load new set of clauses into picosat
  transmit_clauses_vect();

  // 2. Load assumptions into picosat
  transmit_assumptions();

  DBG(FILE* fp = fopen("formula2.cnf", "w");
      IntVector::iterator ipos = iassume->begin();
      IntVector::iterator iend = iassume->end();
      for(; ipos != iend; ++ipos) {
	if (*ipos > 0)      { fprintf(fp, "%ld 0\n", (long int)*ipos); }
	else if (*ipos < 0) { fprintf(fp, "%ld 0\n", (long int)*ipos); }
      } fclose(fp); //);
      static int cnt=0;
      string fname("formula"); fname += stringit(++cnt); fname += ".cnf";
      print_cnf(fname.c_str()); );

  // 3. Invoke SAT solver
  if (verbosity > 0) { prt_std_cputime("c ", "Running SAT solver ..."); }

  picosat_set_global_default_phase(1);      // Specify var phase
  picosat_set_seed(0);
  int status = picosat_sat(-1);    // No limit on decisions
  DBG(cout<<"Status: "<<status<<endl; cout<<"Max var ID: "<<maxvid<<endl;);
  if (verbosity > 0) { prt_std_cputime("c ", "Done running SAT solver ..."); }

  // 4. Analyze outcome and get relevant data
  if (status == 20) {             // Unsatisfiable
    handle_unsat_outcome(); }
  else if (status == 10) {        // Satisfiable
    handle_sat_outcome(); }
  else { Abort("Invalid SAT solver outcome. Contact author."); }

  // 5. Stats & Cleanup
  //picosat_stats();
  //picosat_remove_learned(100); picosat_reset_scores(); picosat_reset_phases();
  if (ret_vect->size() <= CORE_THRESHOLD) {
    picosat_remove_learned(20); picosat_reset_scores(); picosat_reset_phases();
  }
  else {
    picosat_remove_learned(80);picosat_reset_scores();picosat_reset_phases();
  }
  iclvect = NULL; iassume = NULL; ret_vect = NULL;
  return status;
}

int
PicosatIncrementalWrapper::solve(BasicClauseSet& _iclset, 
				 IntVector& _iassume, IntVector& _ret_vect,
				 BasicClauseSet& _corecls)
{
  assert(iclset == NULL && iassume == NULL && ret_vect == NULL);
  corecls = &_corecls;
  int status = solve(_iclset, _iassume, _ret_vect);
  corecls = NULL;
  return status;
}

int
PicosatIncrementalWrapper::solve(BasicClauseVector& _iclvect,
				 IntVector& _iassume, IntVector& _ret_vect,
				 BasicClauseSet& _corecls)
{
  assert(iclvect == NULL && iassume == NULL && ret_vect == NULL);
  corecls = &_corecls;
  int status = solve(_iclvect, _iassume, _ret_vect);
  corecls = NULL;
  return status;
}

void PicosatIncrementalWrapper::transmit_clauses_set()
{
  // Transmit CNF formula to picosat
  DBG(cout<<"Exporting clset w/ size: "<<iclset->size()<<endl;);
  BasicClauseSet& clset = *iclset;
  idcls.resize(idcls.size()+clset.size(), NULL);

  // jpms:debug
  //ofstream couts("current-formula.cnf");
  //couts << "p cnf " << maxvid << " " << clset.get_num_cls() << endl;

  for (cset_iterator cpos = clset.begin(); cpos != clset.end(); ++cpos) {
    BasicClause* cl = *cpos;
    assert(cl->size() > 0);
    // jpms:debug
    //if (cl->size() <= 0) { cout << "Adding empty clause??" << endl; }
    NDBG(cout << "Adding clause to picosat: " << *cl << endl; cout.flush(););

    for (Literator lpos = cl->begin(); lpos != cl->end(); ++lpos) {
      //cout << "  Adding literal: " << *lpos << endl;
      picosat_add(*lpos);
      update_maxvid(*lpos);
      if (abs(*lpos) > varlim) { picosat_set_less_important_lit(*lpos); }
      //else                     { picosat_set_more_important_lit(*lpos); }
    }
    int clid = picosat_add(0);
    idcls[clid] = cl;

    // jpms:debug
    //couts << *cl << "\n";
  }
  // jpms:debug
  if (0) { print_cnf("picoex-cnf.cnf"); }
}

void PicosatIncrementalWrapper::transmit_clauses_vect()
{
  // Transmit CNF formula to picosat
  BasicClauseVector& clvect = *iclvect;
  idcls.resize(idcls.size()+clvect.size(), NULL);
  DBG(cout<<"Exporting clset w/ size: "<<clvect.size()<<endl;);
  // jpms:debug
  //ofstream couts("current-formula.cnf");
  //couts << "p cnf " << imgr.top_id()+1 << " " << clset.get_num_cls() << endl;

  //BasicClauseVector::reverse_iterator clpos = clvect.rbegin();
  //BasicClauseVector::reverse_iterator clend = clvect.rend();
  BasicClauseVector::iterator clpos = clvect.begin();
  BasicClauseVector::iterator clend = clvect.end();
  for(; clpos != clend; ++clpos) {
    BasicClause* cl = *clpos;
    assert(cl->size() > 0);
    // jpms:debug
    //if (cl->size() <= 0) { cout << "Adding empty clause??" << endl; }
    NDBG(cout << "Adding clause to picosat: " << *cl << endl; cout.flush(););
    for (Literator lpos = cl->begin(); lpos != cl->end(); ++lpos) {
      //cout << "  Adding literal: " << *lpos << endl;
      picosat_add(*lpos);
      update_maxvid(*lpos);
      if (abs(*lpos) > varlim) { picosat_set_less_important_lit(*lpos); }
      //else                     { picosat_set_more_important_lit(*lpos); }
    }
    int clid = picosat_add(0);
    idcls[clid] = cl;
    //couts << *cl << "\n";
  }
  // jpms:debug
  if (0) { print_cnf("picoex-cnf.cnf"); }
}

void PicosatIncrementalWrapper::transmit_assumptions()
{
  // Transmit assumptions to SAT solver
  IntVector& assume = *iassume;
  IntVector::iterator apos = assume.begin();
  IntVector::iterator aend = assume.end();
  for (; apos != aend; ++apos) { picosat_assume (*apos); }
}

void PicosatIncrementalWrapper::handle_unsat_outcome()
{
  if (iassume != NULL && iassume->size() > 0) { // If assumptions were specified
    get_core_assumptions();
  }
  if (ccls && corecls != NULL) {    // Get standard core clauses, if applicable
    get_core_clauses();
  }
}

void PicosatIncrementalWrapper::handle_sat_outcome()
{
  IntVector& model = *ret_vect;
  if (model.size() < maxvid+1) { model.resize(maxvid+1, 0); }
  for(ULINT i=1; i<=maxvid; ++i) {
    model[i] = picosat_deref(i);
    DBG(cout << "id " << i << "=" << picosat_deref(i) << endl;);
  }
  NDBG(cout<<"Model size: "<<model.size()<<endl;
      cout<<"INT_MAX:  "<<INT_MAX<<endl;
      cout<<"LONG_MAX: "<<LONG_MAX<<endl;
      FILE* fp = fopen("formula2.cnf", "w");
      for(ULINT i=1; i<=maxvid; ++i) {
	if (model[i] > 0)      { fprintf(fp, "%lu 0\n", (long int)i); }
	else if (model[i] < 0) { fprintf(fp, "%lu 0\n", (long int)i); }
      } fclose(fp); print_cnf("formula.cnf"););
}

void PicosatIncrementalWrapper::get_core_assumptions()
{
  IntVector& assume = *iassume;
  assert(assume.size() > 0);
  IntVector& core_assumes = *ret_vect;
  //if (core_assumes.size() < maxvid+1) { core_assumes.resize(maxvid+1, 0); }
  assert(core_assumes.size() == 0);
  IntVector::iterator apos = assume.begin();
  IntVector::iterator aend = assume.end();
  //core_assumes[0] = 0;    // First entry indicates how many assumptions marked
  for (; apos != aend; ++apos) {
    CHK(if (picosat_failed_assumption(*apos) != picosat_corelit(*apos)) {
	cout << "Lit: " << *apos;
	cout << " FA: " << picosat_failed_assumption(*apos);
	cout << "CL: " << picosat_corelit(*apos) << endl; });
    //if (picosat_corelit(*apos)) { 
    if (picosat_failed_assumption(*apos)) { 
      // Use index in first position to store another core assumption
      //core_assumes[0]++; core_assumes[core_assumes[0]] = *apos; }
      core_assumes.push_back(*apos); }
  }
}

void PicosatIncrementalWrapper::get_core_clauses()
{
  assert(corecls != NULL);
  BasicClauseVector::iterator clpos = idcls.begin();
  for(unsigned int clid=0; clid<idcls.size(); ++clid) {
    if (picosat_coreclause(clid) != 0) {
      corecls->insert_clause(idcls[clid]);
    }
  }
  if (0) {
    FILE* fp = fopen("picoex-core.out", "w");
    printf("Exporting core...\n");
    picosat_write_clausal_core (fp);
    fclose(fp);
  }
}

void PicosatIncrementalWrapper::get_var_levels(IntVector& varlevel)
{
  if (varlevel.size() < maxvid+1) { varlevel.resize(maxvid+1, 0); }
  for(ULINT i=1; i<=maxvid; ++i) {
    varlevel[i] = picosat_deref_level(i);
    DBG(cout << "level(" << i << ")=" << picosat_deref_level(i) << endl;);
  }
}

bool PicosatIncrementalWrapper::is_dvar(LINT vid)
{
  return picosat_decision_lit(vid) != 0;
}

void PicosatIncrementalWrapper::print_cnf(const char* fname)
{
  FILE* fp = fopen(fname, "w");
  picosat_print(fp);
  fclose(fp);
}

/*----------------------------------------------------------------------------*/
