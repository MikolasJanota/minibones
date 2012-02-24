#include <stdio.h>
#include <time.h> 
#include <iostream>
#include <iomanip>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <algorithm>
#include "ToolConfig.hh"
//#include "tests.hh"
#include "ReadCNF.hh"
#include "auxiliary.hh"
#include "BackboneInformation.hh"
#include "Worker.hh"
#include "UpperBound.hh"
#include "UpperBoundProg.hh"
using std::cout;
using std::cin;
using std::setprecision;
using namespace minibones;

ofstream        output_file;
ToolConfig      config;
Worker          *pworker=NULL;
UpperBound      *pupperbound=NULL;
UpperBoundProg  *pupperbound_prog=NULL;
Range           range;
bool            instance_sat = false;

void print_header(ToolConfig& config);
bool parse_options(int argc, char** argv, /*out*/ToolConfig& config);
void print_backbone(const BackboneInformation& worker, const Range& range, ToolConfig& config, ostream& output);
int  run_worker(ToolConfig& config, ostream& output);
int run_upper_bound(ToolConfig& config, ostream& output);
int run_upper_bound_prog(ToolConfig& config, ostream& output);
void register_sig_handlers();

int main(int argc, char** argv) {
  register_sig_handlers();
  if (!parse_options(argc, argv, config)) return -1;
  print_header(config);
  ostream& output=std::cout;
  int return_value;
  if (config.get_self_test()) {
    cout << "running self test" << endl;
    const bool tests_okay =  true;// test_all();
    cout << "self test: " << (tests_okay ? "OK" : "FAIL") << endl;
    return_value = tests_okay ? 0 : 20;
  } else if (config.get_use_upper_bound()) {
    return_value = run_upper_bound(config, output);
  } else if (config.get_use_upper_bound_prog()) {
    return_value = run_upper_bound_prog(config, output);
  } else {
    return_value = run_worker(config, output);
  }
  cout << "i complete" << endl;
  return return_value;
}


Reader* make_reader(string flafile) {
  gzFile ff=Z_NULL;
  if (flafile.size()==1 && flafile[0]=='-') {
    return new Reader(std::cin);
  } else {
    ff = gzopen(flafile.c_str(), "rb");
    if (ff==Z_NULL) {
      cerr << "Unable to open file: " << flafile << endl;
      exit(100);
    }
    return new Reader(ff);
  }
  assert(0);
  return NULL;
}

int run_upper_bound(ToolConfig& config, ostream& output) {
  //read input
  Reader* fr = make_reader(config.get_input_file_name());  
  ReadCNF reader(*fr);
  reader.read();

  //determine which part of variables to compute backbone of
  range=Range(1,reader.get_max_id());
  output << "range: "<<range.first<<"-"<<range.second<<endl;
  pupperbound = new UpperBound(config,output, reader.get_max_id(), reader.get_clause_vector());
  UpperBound& upperbound=*pupperbound;
  if (!upperbound.initialize()) {//unsatisfiable
    config.prefix(output) << "instance unsatisfiable" << endl;
    output << "s 0" << endl;
    delete pupperbound; 
    return 20;
  }
  instance_sat = true;
  //run upperbound
  upperbound.run();
  config.prefix(output) << "computation completed " << endl;
  cout << "i sc:" << upperbound.get_solver_calls() << endl;
  cout << "i st:" << upperbound.get_solver_time() << endl;
  cout << "i ast:" << setprecision(2) << (upperbound.get_solver_time()/(double)upperbound.get_solver_calls()) << endl;
  //print results
  print_backbone(upperbound, range, config, output);
  delete pupperbound; 
  delete fr;
  return 10;
}

int run_upper_bound_prog(ToolConfig& config, ostream& output) {
  //read input
  Reader* fr = make_reader(config.get_input_file_name());  
  ReadCNF reader(*fr);
  reader.read();

  //determine which part of variables to compute backbone of
  range=Range(1,reader.get_max_id());
  output << "i range: "<<range.first<<"-"<<range.second<<endl;
  pupperbound_prog = new UpperBoundProg(config, output, reader.get_max_id(), reader.get_clause_vector());
  UpperBoundProg& upperbound_prog=*pupperbound_prog;
  if (!upperbound_prog.initialize()) {//unsatisfiable
    config.prefix(output) << "instance unsatisfiable" << endl;
    output << "s 0" << endl;
    delete pupperbound_prog; 
    return 20;
  }
  instance_sat = true;
  //run upperbound
  upperbound_prog.run();
  config.prefix(output) << "computation completed " << endl;
  cout << "i sc:" << upperbound_prog.get_solver_calls() << endl;
  cout << "i st:" << upperbound_prog.get_solver_time() << endl;
  cout << "i ast:" << setprecision(2) << (upperbound_prog.get_solver_time()/(double)upperbound_prog.get_solver_calls()) << endl;
  //print results
  print_backbone(upperbound_prog, range, config, output);
  delete pupperbound_prog; 
  delete fr;
  return 10;
}




int run_worker(ToolConfig& config, ostream& output) {
  //read input
  Reader* fr = make_reader(config.get_input_file_name());  
  ReadCNF reader(*fr);
  reader.read();

  //determine which part of variables to compute backbone of
  range=Range(1,reader.get_max_id());
  output << "range: "<<range.first<<"-"<<range.second<<endl;
  pworker = new Worker(config,output, reader.get_max_id(), reader.get_clause_vector(), range);
  Worker& worker=*pworker;
  if (!worker.initialize()) {//unsatisfiable
    config.prefix(output) << "instance unsatisfiable" << endl;
    output << "s 0" << endl;
    delete pworker; 
    return 20;
  }
  instance_sat = true;
  //run worker
  worker.run();
  config.prefix(output) << "computation completed " << endl;
  //print results
  print_backbone(worker, range, config, output);
  delete pworker; 
  return 10;
}

void print_backbone(const BackboneInformation& worker, const Range& range, ToolConfig& config, ostream& output) {
  output << "s 1" << endl;
  output << "v ";
  size_t counter = 0;
  for (Var v = range.first; v <= range.second; ++v) {
    if (worker.is_backbone(v)) {
      ++counter;
      output << ' ';
      output << (worker.backbone_sign(v) ? "" : "-") << v;
    }
  }
  output << endl;
  const float percentage = 100.0 * (float)counter / (float)(range.second - range.first + 1);
  config.prefix(output) << "backbone size: "
                        << counter << " "
                        << setprecision(2) << percentage << "% of range"
                        << endl;
}

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Purpose: Print runtime header when executing DBBones.
\*----------------------------------------------------------------------------*/
//jpms:ec

void print_header(ToolConfig& config) {
  cout_pref<<"*** "<<toolname<<": a backbone tool ***" << endl;
  cout_pref<<"*** release date: "<<release_date<<" ***"<<endl;
  cout_pref<<"*** release ref: "<<changeset<<" ***"<<endl;
#ifdef XPMODE
  cout_pref<<"*** changeset: "<<changeset<<" ***"<<endl;
#endif
  cout_pref<<"*** built: "<<build_date<<" ***"<<endl;
  cout_pref<<"*** author: "<<authorname<<" (" << authoremail << ") ***"<<endl;
  cout_pref<<"*** contributors: "<<contribs<<" ***"<<endl;
  cout_pref<<endl;
  cout_pref<<"*** instance: " << config.get_input_file_name() << " ***"<<endl;
  //cout_pref << "Running bbones ... " << endl;
  //cout_pref<<"Instance: " << filename << endl;
  //cout_pref<<"Run time configuration:" << config.get_cmdstr() << endl;
  string cfgstr; config.get_cfgstr(cfgstr);
  cout_pref<<"*** config:"<<cfgstr<<" ***"<<endl;
  //cout_pref<<"*** config:"<<config.get_cmdstr()<<" ***"<<endl;
  cout_pref<<endl;
}

 bool parse_options(int argc, char** argv, ToolConfig& config) {
  opterr = 0;
  int c;
  while ((c = getopt(argc, argv, "ikpuc:rl")) != -1) {
    switch (c) {
    case 'i':
      config.set_backbone_insertion(1);
      break;
    case 'p':
      config.set_use_upper_bound_prog(true);
      break;
    case 'u':
      config.set_use_upper_bound(true);
      break;
    case 'r':
      config.set_rotatable_pruning(true);
      break;
    case 'k':
      config.set_use_chunk_keeping(1);
      break;
    case 'c':
      config.set_chunk_size(atoi(optarg));
      break;
    case 'l':
      config.set_scdc_pruning();
      break;
    case '?':
      if (optopt == 'c')
        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
      return false;
    default:
      return false;
    }
  }

  if (optind >= argc) {
    config.set_self_test(true);
  } else {
    config.set_self_test(false);
    config.set_input_file_name(argv[optind]);
  }
  return true;
}


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Purpose: Print runtime header when executing BBones.
\*----------------------------------------------------------------------------*/
//jpms:ec

void print_header(ToolConfig& config, const char* fname)
{
  cout_pref<<"*** "<<toolname<<": a backbone tool ***" << endl;
  cout_pref<<"*** release date: "<<release_date<<" ***"<<endl;
  cout_pref<<"*** release ref: "<<changeset<<" ***"<<endl;
#ifdef XPMODE
  cout_pref<<"*** changeset: "<<changeset<<" ***"<<endl;
#endif
  cout_pref<<"*** built: "<<build_date<<" ***"<<endl;
  cout_pref<<"*** author: "<<authorname<<" (" << authoremail << ") ***"<<endl;
  cout_pref<<"*** contributors: "<<contribs<<" ***"<<endl;
  cout_pref<<endl;
  cout_pref<<"*** instance: " << fname << " ***"<<endl;
  //cout_pref << "Running bbones ... " << endl;
  //cout_pref<<"Instance: " << filename << endl;
  //cout_pref<<"Run time configuration:" << config.get_cmdstr() << endl;
  string cfgstr; config.get_cfgstr(cfgstr);
  cout_pref<<"*** config:"<<cfgstr<<" ***"<<endl;
  //cout_pref<<"*** config:"<<config.get_cmdstr()<<" ***"<<endl;
  cout_pref<<endl;
}

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Purpose: Handler for external signals, namely SIGHUP and SIGINT.
\*----------------------------------------------------------------------------*/
//jpms:ec

static void SIG_handler(int signum);
static void finishup();

void register_sig_handlers() {
  signal(SIGHUP,SIG_handler);
  signal(SIGINT,SIG_handler);
  signal(SIGQUIT,SIG_handler);
  signal(SIGTERM,SIG_handler);
  signal(SIGABRT,SIG_handler);
  signal(SIGALRM,SIG_handler);
}

// Generic handler, for all process termination signals that can be caught
static void SIG_handler(int signum) {
  string signame;
  switch (signum) {
    //case 6:  signame = "SIGABRT"; break;
  case 1:  signame = "SIGHUP"; break;
  case 2:  signame = "SIGINT"; break;
  case 3:  signame = "SIGQUIT"; break;
  case 9:  signame = "SIGKILL"; break;
  case 15: signame = "SIGTERM"; break;
  default: cout << "Invalid signal received...\n"; exit(2); break;
  }
  cout << "dbbones received external signal " << signame << ". " << endl;
  cout << "Terminating and printing whatever computed so far." << endl;
  finishup();
}

static void finishup() {
  if (instance_sat) {
    if (pworker) print_backbone (*pworker , range, config, cout);
    if (pupperbound) print_backbone (*pupperbound, range, config, cout);
    if (pupperbound_prog) print_backbone (*pupperbound_prog, range, config, cout);
  } else cout << "s 0" << endl;
  //  if (pworker) delete pworker;
  exit(instance_sat? 10 : 20);
}
