/*----------------------------------------------------------------------------*\
 * Version: $Id: clause_set.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Simple clause set, to hold constraints.
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _CLAUSE_SET_HH_
#define _CLAUSE_SET_HH_ 1

#include <vector>

using namespace std;


#include "id_manager.hh"

#include "cnf_store.hh"


/*----------------------------------------------------------------------------*\
 * Class: PlainClause
 *
 * Purpose: Clauses used within the ClauseSet class context
\*----------------------------------------------------------------------------*/

class PlainClause {

  friend class ClauseSet;

public:

  typedef vector<int> PlainClauseType;

  typedef PlainClauseType::iterator iterator;

public:

  inline void add_literal(int lit) { clause.push_back(lit); }

  inline iterator begin() { return clause.begin(); }

  inline iterator end() { return clause.end(); }

  inline int size() { return clause.size(); }

public:    // Output functions

  void dump(ostream& outs=cout) {
    vector<int>::iterator lpos = clause.begin();
    vector<int>::iterator lend = clause.end();
    for (; lpos != lend; ++lpos) {
      outs << *lpos << " ";
    }
    outs << endl;
  }

  friend ostream & operator << (ostream& outs, PlainClause& cl) {
    cl.dump(outs);
    return outs;
  }

protected:

  PlainClause() : clause() { }

  ~PlainClause() { clause.clear(); }

protected:

  vector<int> clause;
};


/*----------------------------------------------------------------------------*\
 * Class: ClauseSet
 *
 * Purpose: Simple structural manipulation of a set of clauses
\*----------------------------------------------------------------------------*/

class ClauseSet {

public:

  typedef vector<PlainClause*> ClauseSetType;

  typedef ClauseSetType::iterator iterator;

public:

  ClauseSet() : clset() { }

  ~ClauseSet() { 
    iterator cpos = clset.begin();
    iterator cend = clset.end();
    for(; cpos!=cend; ++cpos) {
      if (*cpos) { delete *cpos; }
    }
    clset.clear();
  }

  inline int add_clause() {
    PlainClause *cl = new PlainClause();
    clset.push_back(cl); return clset.size();
  }

  inline void add_literal(int clid, int lit) {
    assert(clid > 0 && (unsigned)clid < clset.size()+1);
    PlainClause* cl = clset[clid-1];  // *must* always take 1 to clid
    cl->add_literal(lit);
  }

  inline iterator begin() { return clset.begin(); }

  inline iterator end() { return clset.end(); }

  inline int size() { return clset.size(); }

  inline void clear() {
    vector<PlainClause*>::iterator clpos = clset.begin();
    vector<PlainClause*>::iterator clend = clset.end();
    for(; clpos!=clend; ++clpos) {
      delete *clpos;
    }
    clset.clear();
  }

public:    // Output functions

  void dump(ostream& outs=cout) {
    iterator cpos = clset.begin();
    iterator cend = clset.end();
    for (; cpos != cend; ++cpos) {
      outs << **cpos << " ";
    }
    outs << endl;
  }

  friend ostream & operator << (ostream& outs, ClauseSet& cl) {
    cl.dump(outs);
    return outs;
  }

protected:

  ClauseSetType clset;

};


/*----------------------------------------------------------------------------*\
 * Class: ClSetExporter
 *
 * Purpose: Wrapper for exporting clause sets
\*----------------------------------------------------------------------------*/

class ClSetExporter {

public:    // Configure

  ClSetExporter(ClauseSet& nclset) {
    init();
    _clset = &nclset;
  }

  virtual ~ClSetExporter() { }    // Nothing to delete

  inline void set_clref_set(ClRefSet& nclrefs) { _clrefs = &nclrefs; }

  inline void set_clset_mapper(IDMap& nidmap) { _idmap = &nidmap; }

public:    // Use

  virtual int node_id(int bid) {  // *must* be virtual (due to comps)
    int nid = abs(bid);
    assert(!_idmap || _idmap->find(nid) != _idmap->end());
    int rid = nid;
    if (_idmap) {
      IDMap::iterator npos = _idmap->find(nid);
      assert(npos != _idmap->end());
      rid = npos->second;
    }
    DBG(cout<<"Mapping "<<nid<<" into "<<rid<<" in CNF exporter"<<endl;);
    return rid;
  }

  inline void register_clref(ClauseRef cl) {
    if (_clrefs != NULL) { assert(!_clrefs->exists(cl)); _clrefs->insert(cl); }
    CHK(else { assert(0); });
  }

  inline ClauseSet& clset() { return *_clset; }

protected:

  inline void init() {
    _idmap = NULL;
    _clrefs = NULL;
    _clset = NULL;
  }

protected:

  IDMap* _idmap;

  ClRefSet* _clrefs;

  ClauseSet* _clset;

};


/*----------------------------------------------------------------------------*\
 * ClauseSetUtils namespace
\*----------------------------------------------------------------------------*/

namespace ClauseSetUtils {

/*----------------------------------------------------------------------------*\
 * Functions exporting clause set into a CNF consumer
\*----------------------------------------------------------------------------*/

  void export_cnf(ClSetExporter& cexp, CNFStore& consumer, IDMap& cnfids);

  void map_var_ids(ClauseSet& clset,
		   IDManager& tmgr, IDSet& sids, IDSet& fids, IDMap& idmap);

  void map_var_ids(ClauseSet& clset, IDManager& tmgr, IDSet& sids,
		   IDMap& idmap, IDMap& refmap);

  void unmap_var_ids(ClauseSet& clset,
		     IDManager& tmgr, IDSet& sids, IDSet& fids,
		     IDMap& idmap, bool rmall);

};

#endif /* _CLAUSE_SET_HH_ */

/*----------------------------------------------------------------------------*/
