//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_wrapper_incr.cc
 *
 * Description: Incremental wrapper for the PicoSAT SAT solver.
 *
 * Author:      jpms
 *
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#include "globals.hh"
#include "logger.hh"
#include "core_importer.hh"
#include "picosat_wrapper_incr.hh"

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
  extern void picosat_set_default_phase_lit (int lit, int phase);

  extern void picosat_reset_phases (void);
  extern void picosat_reset_scores (void);
  extern void picosat_remove_learned (unsigned percentage);

  extern void picosat_print (FILE *);
  extern void picosat_stats (void);
  extern int picosat_variables (void);
  extern int picosat_added_original_clauses (void);
  extern void picosat_set_verbosity (int new_verbosity_level);

  extern int picosat_toplevel (void);
  extern int picosat_deref_level (int lit);
  extern int picosat_decision_lit (int lit);

}


// Constructor. Calls picosat_init()
PicosatWrapperIncr::PicosatWrapperIncr(IDManager& _imgr) :
  SATSolverWrapper(_imgr), c2a_map() //, a2c_map()
{
  maxvid = 0; verbosity = 0; phase = 2; isvalid = false;
  //picosat_init();
}

// Destructor. Calls picosat_reset()
PicosatWrapperIncr::~PicosatWrapperIncr() {
  //picosat_reset();
  c2a_map.clear(); //a2c_map.clear();
  isvalid = false;
}

void PicosatWrapperIncr::init_all()
{
  picosat_init();
#ifndef NCHECK
  if (traceon) { picosat_enable_trace_generation(); }
#endif

  picosat_set_seed(0);
  //picosat_set_global_default_phase(1);      // Specify var phase
  picosat_set_global_default_phase (phase);
  picosat_set_verbosity (verbosity);
  //picosat_set_verbosity (5);

  maxvid = 0; model.clear(); ucore.clear();
  assert(c2a_map.size() == 0);
  //assert(a2c_map.size() == 0);
}

void PicosatWrapperIncr::init_run()
{
  assert(!isvalid);
  model.clear(); ucore.clear(); isvalid = true;
}

void PicosatWrapperIncr::reset_run()
{
  assert(isvalid);
  model.clear(); ucore.clear(); isvalid = false;
}

void PicosatWrapperIncr::reset_all()
{
  DBG(cout << "incr resetting all w/ map size: " << c2a_map.size() << "...\n";);
  picosat_reset();
  //del_all_clauses();
  model.clear(); ucore.clear(); isvalid = false;
  c2a_map.clear(); assert(c2a_map.size() == 0);
  //a2c_map.clear(); assert(a2c_map.size() == 0); 
}

SATRes PicosatWrapperIncr::solve()
{
  assert(isvalid);
  NDBG(cout<<"Vars in solver:        "<<picosat_variables()<<endl;
       cout<<"Clauses in solver:     "<<picosat_added_original_clauses()<<endl;
       cout<<"Assumptions in solver: "<<c2a_map.size()<<endl;
       );

  // 1. Load assumptions into picosat
  transmit_assumptions();

  // 2. Invoke SAT solver
  //prt_std_cputime("c ", "Running SAT solver ...");
  //picosat_set_global_default_phase(1);      // Specify var phase
  //picosat_set_global_default_phase(0);      // Specify var phase
  picosat_set_global_default_phase(2);      // Specify var phase
  //picosat_set_seed(0);
  int status = picosat_sat(-1);    // No limit on decisions
  DBG(cout<<"Status: "<<status<<endl; cout<<"Max var ID: "<<maxvid<<endl;);
  if (verbosity > 0) { prt_std_cputime("c ", "Done running SAT solver ..."); }

  // 4. Analyze outcome and get relevant data
  if (status == 20) {             // Unsatisfiable
    handle_unsat_outcome();
  }
  else if (status == 10) {        // Satisfiable
    handle_sat_outcome();
  }
  else { Abort("Invalid SAT solver outcome. Contact author."); }

  // 5. Stats & Cleanup
  //picosat_stats();
  /*
  picosat_reset_scores();
  picosat_reset_phases();
  */

  return (status == 10) ? SAT_True : SAT_False;
}

void PicosatWrapperIncr::transmit_assumptions()
{
  Cl2IntMap::iterator cpos = c2a_map.begin();
  Cl2IntMap::iterator cend = c2a_map.end();
  for (; cpos != cend; ++cpos) { picosat_assume (cpos->second); }
}

void PicosatWrapperIncr::handle_sat_outcome()
{
  assert(model.size() == 0);
  if (model.size() < maxvid+1) { model.resize(maxvid+1, 0); }
  for(ULINT i=1; i<=maxvid; ++i) {
    model[i] = picosat_deref(i);
    DBG(cout << "id " << i << "=" << picosat_deref(i) << endl;);
  }
  DBG(cout<<"Model size: "<<model.size()<<endl;
      cout<<"INT_MAX:  "<<INT_MAX<<endl;
      cout<<"LONG_MAX: "<<LONG_MAX<<endl;
      FILE* fp = fopen("formula2.cnf", "w");
      for(int i=1; i<=maxvid; ++i) {
	if (model[i] > 0)      { fprintf(fp, "%d 0\n", i); }
	else if (model[i] < 0) { fprintf(fp, "%d 0\n", i); }
      } fclose(fp); print_cnf("formula.cnf"); );
}

void PicosatWrapperIncr::handle_unsat_outcome()
{
  assert(ucore.size() == 0);
  compute_unsat_core();
  if (0) {
    FILE* fp = fopen("picoex-core.out", "w");
    printf("Exporting core...\n");
    picosat_write_clausal_core (fp);
    fclose(fp);
  }
}

// Copy model to new container
void PicosatWrapperIncr::get_model(IntVector& rmodel)
{
  if (rmodel.size() < model.size()) { rmodel.resize(model.size(), 0); }
  copy(model.begin(), model.end(), rmodel.begin());
}

// Copy unsat core to new container
void PicosatWrapperIncr::get_unsat_core(BasicClauseVector& rucore)
{
  assert(traceon);
  if (rucore.size() < ucore.size()) { rucore.resize(ucore.size(), NULL); }
  copy(ucore.begin(), ucore.end(), rucore.begin());
}

// Compute unsat core given map of assumptions to clauses
void PicosatWrapperIncr::compute_unsat_core()
{
  // Visit map from assumptions to clauses
  Cl2IntMap::iterator cpos = c2a_map.begin();
  Cl2IntMap::iterator cend = c2a_map.end();
  for (; cpos != cend; ++cpos) {
    CHK(if (traceon &&
	    picosat_failed_assumption(cpos->second) !=
	    picosat_corelit(cpos->second)) {
	cout << "Lit: " << cpos->second;
	cout << " FA: " << picosat_failed_assumption(cpos->second);
	cout << "CL: " << picosat_corelit(cpos->second) << endl; });
    if (picosat_failed_assumption(cpos->second)) { 
      assert(cpos->first != NULL);
      ucore.push_back(cpos->first);
    }
  }
}

// Add clause to picosat, with associated assumption
int PicosatWrapperIncr::picosat_add_clause(BasicClause* cl, LINT alit)
{
  Literator lpos = cl->begin();
  Literator lend = cl->end();
  for (; lpos != lend; ++lpos) {
    //cout << "  Adding literal: " << *lpos << endl;
    picosat_add(*lpos); update_maxvid(*lpos);
  }
  picosat_add(alit); update_maxvid(alit);
  int clit = picosat_add(0);
  return clit;
}

// Add clause to picosat, without assumption
int PicosatWrapperIncr::picosat_add_clause(BasicClause* cl)
{
  Literator lpos = cl->begin();
  Literator lend = cl->end();
  for (; lpos != lend; ++lpos) {
    //cout << "  Adding literal: " << *lpos << endl;
    picosat_add(*lpos); update_maxvid(*lpos);
  }
  int clit = picosat_add(0);
  return clit;
}

int PicosatWrapperIncr::picosat_add_unit_clause(LINT lit)
{
  picosat_add(lit); update_maxvid(lit);
  return picosat_add(0);
}

void PicosatWrapperIncr::print_cnf(const char* fname)
{
  FILE* fp = fopen(fname, "w");
  picosat_print(fp);
  fclose(fp);
}

void PicosatWrapperIncr::set_literal_phase(LINT lit, int phase)
{
  picosat_set_default_phase_lit (lit, phase);
}

/*----------------------------------------------------------------------------*/
