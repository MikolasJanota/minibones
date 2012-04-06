//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        cl_utils.hh
 *
 * Description: Clause set utilities.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _CL_UTILS_H
#define _CL_UTILS_H 1

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Namespace: ClSetUtils
 *
 * Purpose: Clause set utilities. Examples include intersection of two clsets.
\*----------------------------------------------------------------------------*/
//jpms:ec

namespace ClSetUtils {

  // Template functions for performing intersection of two sets P and Q,
  // such that Q is a subset of P

  // Version 1: Direct deletion of elements from P
  template <typename C,typename I,typename T,typename U>
  void intersect_clset(T& P, U& Q) {
    CHK(int psz = P.size(); int qsz = Q.size(););
    // Create temporary container
    vector<C> vect;
    vect.resize(P.size()+Q.size(), NULL);
    typename vector<C>::iterator vpos = vect.begin();
    // Fill container with contents of P and Q
    I ppos = P.begin();
    I pend = P.end();
    for(; ppos != pend; ++ppos, ++vpos) {
      assert(vpos != vect.end()); *vpos = *ppos;
    }
    typename U::iterator qpos = Q.begin();
    typename U::iterator qend = Q.end();
    for(; qpos != qend; ++qpos, ++vpos) {
      assert(vpos != vect.end()); *vpos = *qpos;
    }
    // Sort contents of container
    sort(vect.begin(), vect.end());
    // Keep duplicates; remove everything else
    typename vector<C>::iterator vref = vect.begin();
    typename vector<C>::iterator vend = vect.end();
    vpos = vect.begin(); ++vpos;
    for(; vref != vend; vref++, vpos++) {
      if (vpos == vend || *vref != *vpos) {
	assert(P.find(*vref) != P.end());
	P.rm_clause(*vref);
      }
      else { vref++; vpos++; }
    }
    CHK(cout << "Init P size: " << psz << endl;
	cout << "Init Q size: " << qsz << endl;
	cout << " Final P size: " << P.size() << endl);
    assert((LINT)P.size() == qsz);
    vect.clear();
  }

  // Version 2: with callback for removing elements
  template <typename C,typename I,typename E,typename T,typename U,typename V>
  void intersect_clset(T& P, U& Q, E& robj, V rm_elem) {
    CHK(int psz = P.size(); int qsz = Q.size(););
    // Create temporary container
    vector<C> vect;
    vect.resize(P.size()+Q.size(), NULL);
    typename vector<C>::iterator vpos = vect.begin();
    // Fill container with contents of P and Q
    I ppos = P.begin();
    I pend = P.end();
    for(; ppos != pend; ++ppos, ++vpos) {
      assert(vpos != vect.end()); *vpos = *ppos;
    }
    typename U::iterator qpos = Q.begin();
    typename U::iterator qend = Q.end();
    for(; qpos != qend; ++qpos, ++vpos) {
      assert(vpos != vect.end()); *vpos = *qpos;
    }
    // Sort contents of container
    sort(vect.begin(), vect.end());
    // Keep duplicates; remove everything else
    typename vector<C>::iterator vref = vect.begin();
    typename vector<C>::iterator vend = vect.end();
    vpos = vect.begin(); ++vpos;
    for(; vref != vend; vref++, vpos++) {
      if (vpos == vend || *vref != *vpos) { (robj->*rm_elem)(*vref); }
      else                                { vref++; vpos++; }
    }
    CHK(cout << "Init P size: " << psz << endl;
	cout << "Init Q size: " << qsz << endl;
	cout << " Final P size: " << P.size() << endl);
    assert((LINT)P.size() == qsz);
    vect.clear();
  }

  // Export clause set in WCNF format
  template <typename C,typename U>
  void export_wcnf(C& cset, U nv, ostream& outs=cout)
  {
    ULINT topw = cset.get_top();
    outs << "p wcnf " << nv << " " << cset.size() << " " << topw << endl;
    ClSetIterator cpos = cset.begin();
    ClSetIterator cend = cset.end();
    for (; cpos != cend; ++cpos) {
      if (cset.is_cl_soft(*cpos)) {
	outs << "1 " << **cpos << endl;
      }
      else {
	outs << topw << " " << **cpos << endl;
      }
    }
  }

}

#endif /* _CL_UTILS_H */

/*----------------------------------------------------------------------------*/
