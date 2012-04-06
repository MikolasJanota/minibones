//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        cl_functors.hh
 *
 * Description: Functors based on clauses.
 *
 * Author:      jpms
 *
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _CL_FUNCTORS_H
#define _CL_FUNCTORS_H 1

#include "basic_clause.hh"


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Functors based on class BasicClause
\*----------------------------------------------------------------------------*/
//jpms:ec

class ClauseHash {
public:
  ULINT operator()(BasicClause* clref) const {  // see below new hash functions
    //return clref->clhash(); // -> Very slow; unable to undertand why
    register ULINT hashv = 0;
    for(Literator pos = clref->begin(); pos != clref->end(); ++pos) {
      hashv ^= (*pos>0) ? *pos : -*pos;
    }
    return hashv;
  }
};

class ClauseEqual {
public:
  bool operator()(BasicClause* cl1, BasicClause* cl2) const {
    if (cl1->size() != cl2->size()) { return false; }
    Literator pos1 = cl1->begin();  // Vectors assumed to be sorted
    Literator pos2 = cl2->begin();
    for(; pos1 != cl1->end(); ++pos1, ++pos2) {
      if (*pos1 != *pos2) { return false; }
    }
    return true;
  }
};

class ClPtrHash {
public:
  ULINT operator()(const BasicClause* ptr) const {
#if USE_CLAUSE_HASH
    // ANTON
    // if defined, the clause hash value is calculated based on content, not
    // the pointer -- give more predictable results; this code is taken from
    // ClauseHash
    register ULINT hashv = 0;
    BasicClause* clref = (BasicClause*)ptr;
    for(Literator pos = clref->begin(); pos != clref->end(); ++pos) {
      hashv ^= (*pos>0) ? *pos : -*pos;
    }
    return hashv;
#else
    return (ULINT)ptr;
#endif
  }
};

class ClPtrEqual {
public:
  bool operator()(const BasicClause* ptr1, const BasicClause* ptr2) const {
    return ptr1 == ptr2;
  }
};

class ClSizeIDLess {
public:
  bool operator()(BasicClause* ptr1, BasicClause* ptr2) const {
    return
      (ptr1->size() < ptr2->size()) ||
      (ptr1->size() == ptr2->size() && ptr1->get_id() < ptr2->get_id());
  }
};

class ClSizeIDGreater {
public:
  bool operator()(BasicClause* ptr1, BasicClause* ptr2) const {
    return
      (ptr1->size() > ptr2->size()) ||
      (ptr1->size() == ptr2->size() && ptr1->get_id() > ptr2->get_id());
  }
};

class ClIDLess {
public:
  bool operator()(BasicClause* ptr1, BasicClause* ptr2) const
  {
    return ptr1->get_id() < ptr2->get_id();
  }
};

class ClIDGreater {
public:
  bool operator()(BasicClause* ptr1, BasicClause* ptr2) const
  {
    return ptr1->get_id() > ptr2->get_id();
  }
};


#define HPSHIFT 3
#define HNSHIFT 2

class LitVectHash {
public:
  ULINT operator()(vector<LINT>* lvect) const {
    //cout << "VECT SIZE:" << lvect->size() <<endl;
    register ULINT hashv = 0;
    //register ULINT hashv = 1;
    //cout << "Hash value: " << hashv << endl; cout.flush();
    for(vector<LINT>::iterator pos=lvect->begin(); pos!=lvect->end(); ++pos) {
      // v3:
      hashv = (hashv << HPSHIFT) ^ ((*pos>0) ? *pos : -*pos);
      /*
      // v5:
      hashv = (hashv << HPSHIFT) ^ (*pos);
      // v4:
      hashv =
      (*pos>0) ? ((hashv << HPSHIFT) ^ *pos) : ((hashv >> HNSHIFT) ^ -*pos);
      // v3:
      hashv = (hashv << HPSHIFT) ^ ((*pos>0) ? *pos : -*pos);
      // v2:
      hashv ^= ((*pos>0) ? *pos : -*pos);
      // v1:
      hashv *= fabs(*pos);
      */
    } //cout << "Hash value: " << hashv << endl; cout.flush();
    return hashv;
  }
};

class LitVectEqual {
public:
  bool operator()(vector<LINT>* lv1, vector<LINT>* lv2) const {
    if (lv1->size() != lv2->size()) { return false; }
    Literator pos1 = lv1->begin();  // Vectors assumed to be sorted
    Literator pos2 = lv2->begin();
    for(; pos1 != lv1->end(); ++pos1, ++pos2) {  // vectors w/ the same size
      if (*pos1 != *pos2) { return false; }
    }
    /* Not as efficient...
      for(register int k=0; k<lv1->size(); ++k) {    // vectors w/ the same size
      if ((*lv1)[k] != (*lv2)[k]) { return false; }
      } //cout<<((pos1==lv1->end()&&pos2==lv2->end())?"TRUE":"FALSE B")<<endl;
      // Also tried reverse iterators, to no avail
    */
    return true;
  }
};

#endif /* _CL_FUNCTORS_H */

/*----------------------------------------------------------------------------*/
