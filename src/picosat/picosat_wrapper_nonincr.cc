//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_wrapper_nonincr.cc
 *
 * Description: Non-incremental wrapper for PicoSAT SAT solver.
 *
 * Author:      jpms
 *
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#include "globals.hh"
#include "logger.hh"
#include "core_importer.hh"
#include "picosat_wrapper_nonincr.hh"

extern "C" {
  extern void picosat_init (void);               /* constructor */
  extern void picosat_reset (void);              /* destructor */

  extern void picosat_enter ();
  extern void picosat_leave ();

  extern void picosat_adjust (int max_idx);

  extern void picosat_enable_trace_generation (void);

  extern int picosat_coreclause(int i);
  extern void picosat_write_clausal_core (FILE * core_file);

  extern int picosat_add (int lit);
  extern int picosat_sat (int decision_limit);
  extern int picosat_deref (int lit);
  extern int picosat_inconsistent();

  extern void picosat_set_seed (unsigned random_number_generator_seed);
  extern void picosat_set_global_default_phase (int);
  extern void picosat_set_default_phase_lit (int lit, int phase);

  extern void picosat_print (FILE *);
  extern void picosat_stats (void);
  extern int picosat_variables (void);
  extern int picosat_added_original_clauses (void);
  extern void picosat_set_verbosity (int new_verbosity_level);

  extern int picosat_toplevel (void);
  extern int picosat_deref_level (int lit);
  extern int picosat_decision_lit (int lit);

}


// Initialize all internal data structures
void PicosatWrapperNonIncr::init_all()
{
  if (!initok) {
    picosat_init();
    if (traceon) { picosat_enable_trace_generation(); }
  }
  model.clear(); ucore.clear(); idcls.clear(); clset.clear();
  initok = true;
}

// Initialize data structures for SAT run
void PicosatWrapperNonIncr::init_run()
{
  if (!initok) {
    picosat_init();
    if (traceon) { picosat_enable_trace_generation(); }
  }
  model.clear(); ucore.clear(); idcls.clear();
  initok = true;
}

// Clean up data structures from SAT run
void PicosatWrapperNonIncr::reset_run()
{
  if (initok) { picosat_reset(); }
  model.clear(); ucore.clear(); idcls.clear();
  initok = false;
}

// Clean up all internal data structures
void PicosatWrapperNonIncr::reset_all()
{
  DBG(cout<<"non-incr resetting all w/ clset size: "<<clset.size()<<"...\n";);
  if (initok) { picosat_reset(); }
  model.clear(); ucore.clear(); idcls.clear(); clset.clear();
  initok = false;
}

SATRes PicosatWrapperNonIncr::solve()
{
  maxvid = 0;
  idcls.resize(clset.size(), NULL);

  // Export CNF formula to picosat
  DBG(cout<<"Exporting clset w/ size: "<<clset.size()<<endl; cout.flush(););
  // jpms:debug
  //ofstream couts("current-formula.cnf");
  //couts << "p cnf " << imgr.top_id()+1 << " " << clset.get_num_cls() << endl;

  ClSetIterator cpos = clset.begin();
  ClSetIterator cend = clset.end();
  for (; cpos != cend; ++cpos) {
    BasicClause* cl = *cpos;
    assert(cl->size() > 0);
    // jpms:debug
    //if (cl->size() <= 0) { cout << "Adding empty clause??" << endl; }
    NDBG(cout << "Adding clause to picosat: " << *cl << endl; cout.flush(););
    Literator lpos = cl->begin();
    Literator lend = cl->end();
    for (; lpos != lend; ++lpos) {
      //cout << "  Adding literal: " << *lpos << endl;
      picosat_add(*lpos);
      //if (fabs(*lpos) > maxvid) { maxvid = fabs(*lpos); }
      update_maxvid(*lpos);
    }
    int clid = picosat_add(0);
    idcls[clid] = cl;
    //couts << *cl << "\n";
  }
  // jpms:debug
  if (0) { print_cnf("picoex-cnf.cnf"); }

  picosat_set_global_default_phase(1);
  // //picosat_set_seed((unsigned)int(RUSAGE::read_cpu_time()));
  //picosat_set_seed((unsigned)clset.size());  // jpms:20091123: removed 

  int status = -1;
  // Due to some inconsistency, not calling picosat_sat if already inconsistent
  // jpms:20090416 -> must enter unsat state with new API.
  //if (!picosat_inconsistent()) { } else { status = 20; }
  status = picosat_sat(-1);    // No limit on decisions

  DBG(cout << "Status: " << status << endl;
      cout << "Max var ID: " << maxvid << endl;);
  // Import unsat core from picosat
  if (status == 20) {                                           // Unsatisfiable
    //cout << "Recording unsat core clauses...\n" << endl;
    if (traceon) { handle_unsat_outcome(); }
  }
  else if (status == 10) {                                        // Satisfiable
    //cout << "Recording computed model...\n" << endl;
    handle_sat_outcome();
  }
  else { tool_abort("Invalid SAT solver outcome. Contact author."); }
  //picosat_stats();

  // jpms:debug
  //  print_cnf("picoex-cnf2.cnf");

  return (status == 10) ? SAT_True : SAT_False;
}

void PicosatWrapperNonIncr::handle_sat_outcome()
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

void PicosatWrapperNonIncr::handle_unsat_outcome()
{
  assert(ucore.size() == 0);
  for(unsigned int clid=0; clid<idcls.size(); ++clid) {
    if (picosat_coreclause(clid) != 0) {
      ucore.push_back(idcls[clid]);
    }
  }
  if (0) {
    FILE* fp = fopen("picoex-core.out", "w");
    printf("Exporting core...\n");
    picosat_write_clausal_core (fp);
    fclose(fp);
  }
}

void PicosatWrapperNonIncr::print_cnf(const char* fname)
{
  FILE* fp = fopen(fname, "w");
  picosat_print(fp);
  fclose(fp);
}

// Copy model to new container
void PicosatWrapperNonIncr::get_model(IntVector& rmodel)
{
  if (rmodel.size() < model.size()) { rmodel.resize(model.size(), 0); }
  copy(model.begin(), model.end(), rmodel.begin());
}

// Copy unsat core to new container
void PicosatWrapperNonIncr::get_unsat_core(BasicClauseVector& rucore)
{
  assert(traceon);
  if (rucore.size() < ucore.size()) { rucore.resize(ucore.size(), NULL); }
  copy(ucore.begin(), ucore.end(), rucore.begin());
}

void PicosatWrapperNonIncr::set_literal_phase(LINT lit, int phase)
{
  picosat_set_default_phase_lit (lit, phase);
}

/*----------------------------------------------------------------------------*/
