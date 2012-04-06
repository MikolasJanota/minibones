//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        cl_types.hh
 *
 * Description: Types derived from clauses and clause sets.
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _CL_TYPES_H
#define _CL_TYPES_H 1

#include "basic_clause.hh"
#include "cl_functors.hh"


typedef enum ClTypes {
  CL_HARD = 0x10,
  CL_SOFT = 0x20,
  CL_WEIGHTED = 0x30
} ClauseTypes;


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Type definitions.
\*----------------------------------------------------------------------------*/
//jpms:ec

class BasicClauseSet;

typedef BasicClause* BasicClausePtr;
typedef BasicClause** BasicClausePtrPtr;

//typedef hash_set<BasicClause*,ClauseHash,ClauseEqual> HashedClauseSet;
typedef hash_set<BasicClause*,ClPtrHash,ClPtrEqual> HashedClauseSet;
typedef HashedClauseSet::iterator ClSetIterator;
typedef HashedClauseSet::iterator cset_iterator;
typedef HashedClauseSet::const_iterator cset_citerator;

typedef hash_map<BasicClause*,LINT,ClPtrHash,ClPtrEqual> Clause2IntMap;

typedef hash_map<IntVector*,BasicClause*,LitVectHash,LitVectEqual> IVec2ClMap;
typedef IVec2ClMap::iterator iv2cl_iterator;

typedef vector<BasicClause*> BasicClauseVector;
typedef BasicClauseVector::iterator ClVectIterator;
typedef BasicClauseVector::iterator cvec_iterator;
typedef BasicClauseVector::const_iterator cvec_citerator;

typedef slist<BasicClause*> BasicClauseSList;

typedef list<BasicClause*> BasicClauseList;

typedef vector<BasicClauseSet*> BasicClauseSetVector;

typedef IntKeyMap<vector<BasicClause*>*> Int2ClVectMap;
//typedef hash_map<ULINT,vector<BasicClause*>*,IntHash,IntEqual> Int2ClVectMap;

typedef hash_map<BasicClause*,vector<LINT>*,ClPtrHash,ClPtrEqual> Cl2IntVMap;

typedef hash_map<BasicClause*,vector<BasicClause*>*,ClPtrHash,ClPtrEqual> Cl2ClVMap;

typedef hash_map<BasicClause*,LINT,ClPtrHash,ClPtrEqual> Cl2IntMap;
typedef Cl2IntMap::iterator c2n_iterator;

typedef hash_map<LINT,BasicClause*,IntHash,IntEqual> Int2ClMap;

//typedef hash_map<LINT,vector<LINT>*,IntHash,IntEqual> Int2IntVMap;

typedef hash_map<BasicClause*,BasicClause*,ClPtrHash,ClPtrEqual> Cl2ClMap;

typedef
vector<BasicClauseSet*> ClauseSetVector;

typedef 
hash_map<LINT,BasicClauseVector*,IntHash,IntEqual> Int2ClVMap;

typedef 
hash_map<XLINT,BasicClauseVector*,XLIntHash,XLIntEqual> XLInt2ClVMap;

typedef
hash_map<BasicClause*,vector<LINT>*,ClPtrHash,ClPtrEqual> Cl2IntVMap;

typedef
hash_map<BasicClause*,vector<BasicClause*>*,ClPtrHash,ClPtrEqual> Cl2ClVMap;

typedef
hash_map<BasicClause*,LINT,ClPtrHash,ClPtrEqual> Cl2IntMap;

typedef
hash_map<LINT,BasicClause*,IntHash,IntEqual> Int2ClMap;

typedef
hash_map<LINT,vector<LINT>*,IntHash,IntEqual> Int2IntVMap;

//typedef
//hash_map<LINT,BasicClause*,IntHash,IntEqual> Int2ClVMap;

typedef 
vector<Cl2IntMap*> VectCl2IntMap;

typedef
vector<HashedClauseSet*> VectClSet;

typedef
vector<HashedClauseSet> VectHashedClauseSet;

//typedef
//vector<hash_map<BasicClause*,LINT,ClPtrHash,ClPtrEqual>*> IntVCl2IntMap;

//typedef hash_map<LINT,vector<BasicClause*>,IntHash,IntEqual> Int2ClVMap;

//typedef
//hash_map<BasicClausePtr,BasicClausePtrPtr,ClPtrHash,ClPtrEqual> Cl2ClPMap;

#endif /* _CL_TYPES_H */

/*----------------------------------------------------------------------------*/
