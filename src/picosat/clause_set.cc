/*----------------------------------------------------------------------------*\
 * Version: $Id: clause_set.cc 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Functions used by the clause set and related classes.
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#include "clause_set.hh"


void ClauseSetUtils::export_cnf(ClSetExporter& cexp,
				CNFStore& consumer, IDMap& cnfids)
{
  DBG(cout << "Exporting clause set..." << endl;);
  vector<int> ncl;    // Clauses to register
  ClauseSet& clset = cexp.clset();
  ClauseSet::iterator cpos = clset.begin();
  ClauseSet::iterator cend = clset.end();
  for (; cpos!=cend; ++cpos) {
    PlainClause* cl = *cpos;
    DBG(cout << "Exporting clause in clause set: " << *cl << endl;);
    PlainClause::iterator lpos = cl->begin();
    PlainClause::iterator lend = cl->end();
    for (; lpos!=lend; ++lpos) {
      int sign = (*lpos<0) ? 1 : 0;
      int vid = (*lpos<0) ? -*lpos : *lpos;
      int mid = cexp.node_id(vid);    // Map from base domain

      IDMap::iterator hpos = cnfids.find(mid);  // map into SAT domain
      int rid = -1;
      if (hpos == cnfids.end()) { 
	rid = consumer.new_var();
	cnfids.insert(make_pair(mid, rid));
	DBG(cout<<"Associating "<<rid<<" with "<<mid<<endl;);
      }
      else { rid = hpos->second; }

      DBG(cout<<"Adding literal "<<((sign)? -rid : rid)<<endl;);
      ncl.push_back((sign)? -rid : rid);
    }
    assert(ncl.size() > 0); DBG(PRINT_ELEMENTS(ncl, "Adding clause: "););
    ClauseRef clref = consumer.add_clause(ncl); ncl.clear();
    assert(clref != NULL);
    cexp.register_clref(clref);
  }
}

// Map id's, from existing id's to id's from a target id manager
void ClauseSetUtils::map_var_ids(ClauseSet& clset, IDManager& tmgr,
				 IDSet& sids, IDSet& fids, IDMap& idmap)
{
  ClauseSet::iterator cpos = clset.begin();
  ClauseSet::iterator cend = clset.end();
  for (; cpos!=cend; ++cpos) {
    PlainClause* cl = *cpos;
    DBG(cout << "Mapping id's for clause in clause set: " << *cl << endl;);
    PlainClause::iterator lpos = cl->begin();
    PlainClause::iterator lend = cl->end();
    for (; lpos!=lend; ++lpos) {
      int vid = (*lpos<0) ? -*lpos : *lpos;
      IDSet::iterator fpos = fids.find(vid);
      if (fpos != fids.end()) { continue; }    // Do not map if it's filtered

      IDMap::iterator hpos = idmap.find(vid);  // map into external id's
      if (hpos == idmap.end()) {    // Map only if not already mapped
	int nid = tmgr.new_id();
	idmap.insert(make_pair(vid, nid));
	DBG(cout<<"Mapping "<<vid<<" into "<<nid<<endl;);
      } ASSERTCODE(else if(sids.find(vid) == sids.end()) {
	cout<<"ClSet: Already mapped id "<<vid<<" not shared/filtered?"<<endl;
	assert(0);});
    }
  }
}


// *** TO BE MODIFIED ***

// Map id's, as above, but use refmap as a reference of existing maps
void ClauseSetUtils::map_var_ids(ClauseSet& clset, IDManager& tmgr, IDSet& sids,
				 IDMap& idmap, IDMap& refmap)
{
  assert(0); exit(2);

  ClauseSet::iterator cpos = clset.begin();
  ClauseSet::iterator cend = clset.end();
  for (; cpos!=cend; ++cpos) {
    PlainClause* cl = *cpos;
    DBG(cout << "Mapping id's for clause in clause set: " << *cl << endl;);
    PlainClause::iterator lpos = cl->begin();
    PlainClause::iterator lend = cl->end();
    for (; lpos!=lend; ++lpos) {
      int vid = (*lpos<0) ? -*lpos : *lpos;
      IDMap::iterator hpos = idmap.find(vid);  // map into external id's
      if (hpos == idmap.end()) {
	IDMap::iterator rpos = refmap.find(vid);
	int nid = (rpos == refmap.end()) ? tmgr.new_id() : rpos->second;
	DBG(cout<<"Mapping "<<vid<<" into "<<nid<<endl;);
	idmap.insert(make_pair(vid, nid));
      } ASSERTCODE(else {assert(sids.find(vid) != sids.end());});
    }
  }
}

// Unmap id s, as above, but use refmap as a reference of existing maps
void ClauseSetUtils::unmap_var_ids(ClauseSet& clset,
				   IDManager& tmgr, IDSet& sids, IDSet& fids,
				   IDMap& idmap, bool rmall)
{
  ClauseSet::iterator cpos = clset.begin();
  ClauseSet::iterator cend = clset.end();
  for (; cpos!=cend; ++cpos) {
    PlainClause* cl = *cpos;
    DBG(cout << "Mapping id's for clause in clause set: " << *cl << endl;);
    PlainClause::iterator lpos = cl->begin();
    PlainClause::iterator lend = cl->end();
    for (; lpos!=lend; ++lpos) {
      int vid = (*lpos<0) ? -*lpos : *lpos;
      if (rmall || sids.find(vid) == sids.end()) {
	idmap.erase(vid);
      }
    }
  }
}

/*----------------------------------------------------------------------------*/
