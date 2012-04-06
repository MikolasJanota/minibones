//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        picosat_bridge.cc
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

#include <stdio.h>

#include "globals.hh"
#include "logger.hh"
#include "str_utils.hh"
#include "core_importer.hh"
#include "picosat_bridge.hh"


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

  extern void picosat_print (FILE *);
  extern void picosat_stats (void);
  extern void picosat_set_verbosity (int new_verbosity_level);

  extern int picosat_toplevel (void);
  extern int picosat_deref_level (int lit);
  extern int picosat_decision_lit (int lit);

  extern void picosat_write_compact_trace (FILE * trace_file);
  extern void picosat_write_extended_trace (FILE * trace_file);
}


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Purpose: Interfaces the picosat SAT solver.
\*----------------------------------------------------------------------------*/
//jpms:ec

int PicosatWrapper::run_sat_solver()
{
  maxvid = 0;
  idcls.resize(clset.size(), NULL);

  picosat_init();
  picosat_enable_trace_generation ();

  // Export CNF formula to picosat
  DBG(cout<<"Exporting clset w/ size: "<<clset.size()<<endl;);
  // jpms:debug
  //ofstream couts("current-formula.cnf");
  //couts << "p cnf " << imgr.top_id()+1 << " " << clset.get_num_cls() << endl;

  for (cset_iterator cpos = clset.begin(); cpos != clset.end(); ++cpos) {
    BasicClause* cl = *cpos;
    assert(cl->size() > 0);
    // jpms:debug
    //if (cl->size() <= 0) { cout << "Adding empty clause??" << endl; }
    NDBG(cout << "Adding clause to picosat: " << *cl << endl; cout.flush(););
    for (Literator lpos = cl->begin(); lpos != cl->end(); ++lpos) {
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
  if (status == 20) {         // Unsatisfiable
    compute_unsat_core();
    if (trace_file != NULL) { print_trace(); }
  }
  else if (status == 10) {    // Satisfiable
    // jpms:20090217 -> Add here code for recording computed model
    if (model.size() < maxvid+1) { model.resize(maxvid+1, 0); }
    for(int i=1; i<=maxvid; ++i) {
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

  } else {
    Abort("Invalid SAT solver outcome. Contact author.");
  }
  //picosat_stats();
  picosat_reset();

  // jpms:debug
  //  print_cnf("picoex-cnf2.cnf");

  //cout << "Going to export the core clauses...\n" << endl;
  return status;
}

/*
void PicosatWrapper::get_model(vector<int>& rmodel)
{
  if (rmodel.size() < model.size()) { rmodel.resize(model.size(), 0); }
  copy(model.begin(), model.end(), rmodel.begin());
}
*/

void PicosatWrapper::init() { }

void PicosatWrapper::reset()
{
  //model.clear();
  idcls.clear();
}

void PicosatWrapper::compute_unsat_core()
{
  vector<BasicClause*>::iterator clpos = idcls.begin();
  for(unsigned int clid=0; clid<idcls.size(); ++clid) {
    if (picosat_coreclause(clid) != 0) {
      clvec.push_back(idcls[clid]);
    }
  }
  if (0) {
    FILE* fp = fopen("picoex-core.out", "w");
    printf("Exporting core...\n");
    picosat_write_clausal_core (fp);
    fclose(fp);
  }
}

void PicosatWrapper::get_var_levels(IntVector& varlevel)
{
  if (varlevel.size() < maxvid+1) { varlevel.resize(maxvid+1, 0); }
  for(ULINT i=1; i<=maxvid; ++i) {
    varlevel[i] = picosat_deref_level(i);
    DBG(cout << "level(" << i << ")=" << picosat_deref_level(i) << endl;);
  }
}

bool PicosatWrapper::is_dvar(LINT vid)
{
  return picosat_decision_lit(vid) != 0;
}

void PicosatWrapper::print_trace()
{
  assert(trace_file != NULL);
  niter++;
  string wfname(trace_file);
  wfname += "-f"; wfname += stringit(niter); wfname += ".txt";
  FILE* tracef = fopen(wfname.c_str(), "w");
  assert(tracef != NULL);
  picosat_write_extended_trace (tracef);
  fclose(tracef);
}

void PicosatWrapper::print_cnf(const char* fname)
{
  FILE* fp = fopen(fname, "w");
  assert(fp != NULL);
  picosat_print(fp);
  fclose(fp);
}

/*----------------------------------------------------------------------------*/
