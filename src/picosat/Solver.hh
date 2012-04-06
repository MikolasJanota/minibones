/****************************************************************************************[Solver.h]
MiniSat -- Copyright (c) 2003-2005, Niklas Een, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#ifndef Solver_hh
#define Solver_hh

#include <vector>

using namespace std;


#include "SolverTypes.hh"
#include "VarOrder.hh"

#define reportf(format, args...) ( printf(format , ## args), fflush(stdout) )


#include "globals.hh"
#include "cnf_store.hh"
#include "proof_logger.hh"


//=================================================================================================
// Solver -- the main class:


struct SolverStats {
  int64   starts, decisions, propagations, conflicts;
  int64   clauses_literals, learnts_literals, max_literals, tot_literals;
  int64    clauses_binary, clauses_ternary, clauses_large;
  SolverStats() :
    starts(0), decisions(0), propagations(0), conflicts(0),
    clauses_literals(0), learnts_literals(0), max_literals(0), tot_literals(0),
    clauses_binary(0), clauses_ternary(0), clauses_large(0) { }
};


struct SearchParams {
    double  var_decay, clause_decay, random_var_freq;    // (reasonable values are: 0.95, 0.999, 0.02)    
    SearchParams(double v = 1, double c = 1, double r = 0) : var_decay(v), clause_decay(c), random_var_freq(r) { }
};



class Solver : public CNFStore {
protected:
    // Solver state:
    //
    bool                ok;               // If FALSE, the constraints are already unsatisfiable. No part of the solver state may be used!
  Clause*             empty_cl;           // Empty clause
  Clause*             st_confl;           // Conflict while adding clauses
  int                 unit_cls;           // Number of original unit clauses
  int                 learnt_units;       // Number of learnt unit clauses
  StdClause           clref;              // Clause to return when accessing cls
    vec<Clause*>        clauses;          // List of problem clauses.
    vec<Clause*>        learnts;          // List of learnt clauses.
    int                 n_bin_clauses;    // Keep track of number of binary clauses "inlined" into the watcher lists (we do this primarily to get identical behavior to the version without the binary clauses trick).
    double              cla_inc;          // Amount to bump next clause with.
    double              cla_decay;        // INVERSE decay factor for clause activity: stores 1/decay.

    vec<double>         activity;         // A heuristic measurement of the activity of a variable.
    double              var_inc;          // Amount to bump next variable with.
    double              var_decay;        // INVERSE decay factor for variable activity: stores 1/decay. Use negative value for static variable order.
    VarOrder            order;            // Keeps track of the decision variable order.

    vec<vec<Clause*> >  watches;          // 'watches[lit]' is a list of constraints watching 'lit' (will go there if literal becomes true).
    vec<char>           assigns;          // The current assignments (lbool:s stored as char:s).
    vec<Lit>            trail;            // Assignment stack; stores all assigments made in the order they were made.
    vec<int>            trail_lim;        // Separator indices for different decision levels in 'trail'.
    vec<Clause*>        reason;           // 'reason[var]' is the clause that implied the variables current value, or 'NULL' if none.
    vec<int>            level;            // 'level[var]' is the decision level at which assignment was made.
    int                 root_level;       // Level of first proper decision.
    int                 qhead;            // Head of queue (as index into the trail -- no more explicit propagation queue in MiniSat).
    int                 simpDB_assigns;   // Number of top-level assignments since last execution of 'simplifyDB()'.
    int64               simpDB_props;     // Remaining number of propagations that must be made before next execution of 'simplifyDB()'.

    // Temporaries (to reduce allocation overhead). Each variable is prefixed by the method in which is used:
    //
    vec<char>           analyze_seen;
    vec<Lit>            analyze_stack;
    vec<Lit>            analyze_toclear;
    Clause*             propagate_tmpbin;
    Clause*             analyze_tmpbin;
    Clause*             solve_tmpunit;
    vec<Lit>            addBinary_tmp;
    vec<Lit>            addTernary_tmp;

  // Proof tracing variables
  ProofLogger *plogger;

    // Main internal methods:
    //
    bool        assume           (Lit p);
    void        cancelUntil      (int level);
    void        record           (const vec<Lit>& clause);

    void        analyze          (Clause* confl, vec<Lit>& out_learnt, int& out_btlevel); // (bt = backtrack)
    bool        analyze_removable(Lit p, uint min_level);                                 // (helper method for 'analyze()')
    void        analyzeFinal     (Clause* confl,  bool skip_first = false);
  inline bool   enqueue          (Lit fact, Clause* from = NULL) {
    if (value(fact) != l_Undef)
      return value(fact) != l_False;
    else{
      DBG(cout<<"Assigning "<<var(fact)+1<<" @ "<<decisionLevel()<<endl;);
      assigns[var(fact)] = toInt(lbool(!sign(fact)));
      level  [var(fact)] = decisionLevel();
      reason [var(fact)] = from;
      trail.push(fact);
      return true;
    }
  }
    Clause*     propagate        ();
    void        reduceDB         ();
    Lit         pickBranchLit    (const SearchParams& params);
    lbool       search           (int nof_conflicts, int nof_learnts, const SearchParams& params);
    double      progressEstimate ();

    // Activity:
    //
    void     varBumpActivity(Lit p) {
        if (var_decay < 0) return;     // (negative decay means static variable order -- don't bump)
        if ( (activity[var(p)] += var_inc) > 1e100 ) varRescaleActivity();
        order.update(var(p)); }
    void     varDecayActivity  () { if (var_decay >= 0) var_inc *= var_decay; }
    void     varRescaleActivity();
    void     claDecayActivity  () { cla_inc *= cla_decay; }
    void     claRescaleActivity();

    // Operations on clauses:
    //
    void     newClause(const vec<Lit>& ps, bool learnt = false);
    void     claBumpActivity (Clause* c) { if ( (c->activity() += cla_inc) > 1e20 ) claRescaleActivity(); }
    void     remove          (Clause* c, bool just_dealloc = false);
bool     locked          (const Clause* c) const { return reason[var((*c)[0])] == c; }
    bool     simplify        (Clause* c) const;

    int      decisionLevel() const { return trail_lim.size(); }

public:
    Solver() : ok               (true)
             , st_confl       (NULL)
             , n_bin_clauses    (0)
             , cla_inc          (1)
             , cla_decay        (1)
             , var_inc          (1)
             , var_decay        (1)
             , order            (assigns, activity)
             , qhead            (0)
             , simpDB_assigns   (0)
             , simpDB_props     (0)
//             , default_params   (SearchParams(0.95, 0.999, 0.02))
             , default_params   (SearchParams(0.95, 0.999, 0.0))
             , expensive_ccmin  (true)
             , verbosity        (0)
             , progress_estimate(0)
             {
	       plogger = NULL;
	       unit_cls = 0;
	       learnt_units = 0;
	       new_var();    // Setting 0 as a dummy variable...
	       vec<Lit> dummy(2,lit_Undef);
	       propagate_tmpbin = Clause_new(false, dummy);
	       analyze_tmpbin   = Clause_new(false, dummy);
	       dummy.pop();
	       solve_tmpunit    = Clause_new(false, dummy);
	       addBinary_tmp .growTo(2);
	       addTernary_tmp.growTo(3);
	       assert(trail_lim.size() == 0);
	       vec<Lit> nops;
	       empty_cl = Clause_new(true, nops);  // It *was* learnt
             }

   virtual ~Solver() {
     xfree(empty_cl);
       for (int i = 0; i < learnts.size(); i++) remove(learnts[i], true);
       for (int i = 0; i < clauses.size(); i++) if (clauses[i] != NULL) remove(clauses[i], true); }

    // Helpers: (semi-internal)
    //
    lbool   value(Var x) const { return toLbool(assigns[x]); }
    lbool   value(Lit p) const { return sign(p) ? ~toLbool(assigns[var(p)]) : toLbool(assigns[var(p)]); }

    int     nAssigns() { return trail.size(); }
    int     nClauses() { return clauses.size(); }
    int     nLearnts() { return learnts.size(); }

    // Statistics: (read-only member variable)
    //
    SolverStats     stats;

    // Mode of operation:
    //
    SearchParams    default_params;     // Restart frequency etc.
    bool            expensive_ccmin;    // Controls conflict clause minimization. TRUE by default.
    int             verbosity;          // Verbosity level. 0=silent, 1=some progress report, 2=everything

public:
  // CNFStore interface
  int new_var() { return (int)newVar()+1; } // Obtain identified for new variable
  int num_vars() { return nVars(); }        // Return number of existing variables

  ClauseRef add_clause(vector<int>& vlit);  // Create cl w/ vect of lits

  int num_base_cls() {                               // Original num of cls
    return nClauses(); // + unit_cls;
  }

  int num_learnt_cls() {                             // Number of learnt clauses
    return nLearnts(); // + learnt_units;
  }

  int num_total_cls() {                              // Total number of clauses
    return num_base_cls() + num_learnt_cls();
  }

  StdClause& get_clause(ClauseRef cl);      // Access a clause...

  StdClause& get_clause(int clid);          // Access a clause...

  // Register proof trace object

  void register_proof_logger(ProofLogger* pl) { plogger = pl; }

protected:

  Clause* int_add_clause(vec<Lit>& ps, bool learnt);   // Internal method


    // Problem specification:
    //
    Var     newVar    ();
    int     nVars     ()                    { return assigns.size(); }

public:
    // Solving:
    //
    bool    okay() { return ok; }       // FALSE means solver is in an conflicting state (must never be used again!)
    void    simplifyDB();
    bool    solve(const vec<Lit>& assumps);
    bool    solve() { vec<Lit> tmp; return solve(tmp); }

  void print_stats(bool outcome, ostream& outs);

    double      progress_estimate;  // Set by 'search()'.
    vec<lbool>  model;              // If problem is satisfiable, this vector contains the model (if any).
    vec<Lit>    conflict;           // If problem is unsatisfiable (possibly under assumptions), this vector represent the conflict clause expressed in the assumptions.

public:

};


// Comparison of literals using DLevel info
class dlevel_lt {
public:
  dlevel_lt(vec<int>& nlevel) : level(nlevel) {}
  bool operator () (Lit litA, Lit litB) {
    return level[var(litA)] < level[var(litB)];
  }
protected:
  vec<int>& level;
};

class dlevel_gt {
public:
  dlevel_gt(vec<int>& nlevel) : level(nlevel) {}
  bool operator () (Lit litA, Lit litB) {
    return level[var(litA)] > level[var(litB)];
  }
protected:
  vec<int>& level;
};

//=================================================================================================
// Debug:


#define L_LIT    "%sx%d"
#define L_lit(p) sign(p)?"~":"", var(p)

// Just like 'assert()' but expression will be evaluated in the release version as well.
inline void check(bool expr) { assert(expr); }


//=================================================================================================
#endif
