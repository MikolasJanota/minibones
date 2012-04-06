//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        basic_matrix.hh
 *
 * Description: Implements two-dimensional matrix with basic funcionality.
 *
 * Notes:       It does not work with bool's because of vector<bool> is
 *              specialized in STL.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _BASIC_MATRIX_H
#define _BASIC_MATRIX_H 1

#include <assert.h>
#include <vector>
#include <iostream>

//#include "globals.hh"

using namespace std;


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: BasicMatrix
 *
 * Purpose: Implements two-dimensional matrix with basic funcionality.
\*----------------------------------------------------------------------------*/
//jpms:ec

template <typename T> 
class BasicMatrix {

public:

  BasicMatrix(unsigned int _nr, unsigned int _nc, T _nv) :
    mt(_nr), nv(_nv), nr(_nr), nc(_nc) { make_all_rows(); }

  ~BasicMatrix() { unset_size(); }
  //~BasicMatrix() { erase(); }  // jpms@20101026: alternative

  inline unsigned int nrows() {
    assert(nr == mt.size()); return nr;
  }

  inline unsigned int ncols() {
    assert(nr > 0 && nc == mt[0]->size());
    return nc;
  }

  inline void push_row() { push_row(nc); }

  inline void pop_row() {
    vector<T>* v = mt.back(); mt.pop_back(); delete v; nr--;
  }

  inline vector<T>& top_row() {
    assert(nr > 0); return *mt[nr-1]; }

  inline void push_col() {
    nc++;
    if (nr > 0) { for(unsigned int i=0; i<nr; ++i) { mt[i]->push_back(nv); } }
  }

  inline void pop_col() {
    nc--;
    if (nr > 0) { for(unsigned int i=0; i<nr; ++i) { mt[i]->pop_back(); } }
  }

  T& operator()(unsigned int r, unsigned int c)
  {
    assert(r >= 0 && r < nr); assert(c >= 0 && c < nc);
    return (*mt[r])[c];
  }

  vector<T>& operator[](unsigned int r)
  {
    assert(r >= 0 && r < nr);
    return *mt[r];
  }

  typename vector<T>::iterator row_begin(unsigned int r)
  {
    assert(r >= 0 && r < nr);
    return (*mt[r]).begin();
  }

  typename vector<T>::iterator row_end(unsigned int r)
  {
    assert(r >= 0 && r < nr);
    return (*mt[r]).end();
  }

  typename vector<T>::reverse_iterator row_rbegin(unsigned int r)
  {
    assert(r >= 0 && r < nr);
    return (*mt[r]).rbegin();
  }

  typename vector<T>::reverse_iterator row_rend(unsigned int r)
  {
    assert(r >= 0 && r < nr);
    return (*mt[r]).rend();
  }

  void unset_size()
  {
    for(unsigned int i=0; i<nr; ++i) {
      mt[i]->clear(); delete mt[i]; mt[i] = NULL;
    }
    mt.resize(0);
  }

  void set_size(unsigned int _nr, unsigned int _nc)
  {
    assert(nr == 0);
    nr = _nr; nc = _nc;
    mt.resize(nr, NULL);
    for(unsigned int i=0; i<nr; ++i) { mt[i] = new vector<T>(nc, nv); }
  }

  void dump(ostream& outs=cout) {
    //outs << "(NR,NC)=(" << nr << "," << nc << ")" << endl;
    for(unsigned int i=0; i<nr; ++i) {
      for(unsigned int j=0; j<nc; ++j) {
	outs << (*mt[i])[j] << " ";
      }
      outs << endl;
    }
  }

  void tostr(typename vector<T>::iterator rpos,
	     typename vector<T>::iterator rend, ostream& outs=cout)
  {
    for(; rpos != rend; ++rpos) {
      outs << *rpos;
    }
    outs << endl;
  }

  friend ostream & operator << (ostream& outs, BasicMatrix<T>& mat) {
    mat.dump(outs);
    return outs;
  }

protected:

  inline void push_row(unsigned int _nc) {
    assert(_nc == nc); mt.push_back(new vector<T>(_nc, nv)); nr++; }

  inline void make_all_rows() {
    for(unsigned int i=0; i<mt.size(); ++i) { make_row(i); }
  }

  inline void make_row(unsigned int i) {
    assert(i >= 0 && i < nr); assert(mt[i] == NULL);
    mt[i] = new vector<T>(nc, nv);
  }

  inline void erase() { while(nr > 0) { pop_row(); } }


protected:

  vector<vector<T>* > mt;      // The actual matrix

  T nv;                        // Non-value of type T

  unsigned int nr;             // Number of rows

  unsigned int nc;             // Number of columns

};

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: BasicBoolMatrix
 *
 * Purpose: Implements two-dimensional matrix of bools with basic funcionality.
\*----------------------------------------------------------------------------*/
//jpms:ec

class BasicBoolMatrix {

public:

  BasicBoolMatrix(unsigned int _nr, unsigned int _nc, bool _nv) :
    mt(_nr), nv(_nv), nr(_nr), nc(_nc) { make_all_rows(); }

  ~BasicBoolMatrix() { erase(); }

  inline unsigned int nrows() {
    assert(nr == mt.size()); return nr;
  }

  inline unsigned int ncols() {
    assert(nr > 0 && nc == mt[0]->size());
    return nc;
  }

  inline void push_row() { push_row(nc); }

  inline void pop_row() {
    vector<bool>* v = mt.back(); mt.pop_back(); delete v; nr--;
  }

  inline vector<bool>& top_row() {
    assert(nr > 0); return *mt[nr-1]; }

  inline void push_col() {
    nc++;
    if (nr > 0) { for(unsigned int i=0; i<nr; ++i) { mt[i]->push_back(nv); } }
  }

  inline void pop_col() {
    nc--;
    if (nr > 0) { for(unsigned int i=0; i<nr; ++i) { mt[i]->pop_back(); } }
  }

  bool operator()(unsigned int r, unsigned int c)
  {
    assert(r >= 0 && r < nr); assert(c >= 0 && c < nc);
    return (*mt[r])[c];
  }

  bool get(unsigned int r, unsigned int c)
  {
    assert(r >= 0 && r < nr); assert(c >= 0 && c < nc);
    return (*mt[r])[c];
  }

  void set(unsigned int r, unsigned int c, bool v)
  {
    assert(r >= 0 && r < nr); assert(c >= 0 && c < nc);
    (*mt[r])[c] = v;
  }

  vector<bool>& operator[](unsigned int r)
  {
    assert(r >= 0 && r < nr);
    return *mt[r-1];
  }

  void unset_size()
  {
    for(unsigned int i=0; i<nr; ++i) {
      mt[i]->clear(); delete mt[i]; mt[i] = NULL;
    }
    mt.resize(0);
  }

  void set_size(unsigned int _nr, unsigned int _nc)
  {
    assert(nr == 0);
    nr = _nr; nc = _nc;
    mt.resize(nr, NULL);
    for(unsigned int i=0; i<nr; ++i) {
      mt[i] = new vector<bool>(nc, nv);
      assert(mt[i]->size() == nc);
    }
    assert(mt.size() == nr);
    cout << "(NR, NC)=(" << nr << ", " << nc << ")" << endl;

    assert(nr > 0 && nc == mt[0]->size());
  }

  void dump(ostream& outs=cout) {
    //outs << "(NR,NC)=(" << nr << "," << nc << ")" << endl;
    for(unsigned int i=0; i<nr; ++i) {
      for(unsigned int j=0; j<nc; ++j) {
	outs << (*mt[i])[j] << " ";
      }
      outs << endl;
    }
  }

  friend ostream & operator << (ostream& outs, BasicBoolMatrix& mat) {
    mat.dump(outs);
    return outs;
  }

protected:

  inline void push_row(unsigned int _nc) {
    assert(_nc == nc); mt.push_back(new vector<bool>(_nc, nv)); nr++; }

  inline void make_all_rows() {
    for(unsigned int i=0; i<mt.size(); ++i) { make_row(i); }
  }

  inline void make_row(unsigned int i) {
    assert(i>= 0 && i < nr); assert(mt[i] == NULL);
    mt[i] = new vector<bool>(nc, nv);
  }

  inline void erase() { while(nr > 0) { pop_row(); } }


protected:

  vector<vector<bool>* > mt;   // The actual matrix

  bool nv;                     // Non-value of type bool

  unsigned int nr;             // Number of rows

  unsigned int nc;             // Number of columns

};

#endif /* _BASIC_MATRIX_H */

/*----------------------------------------------------------------------------*/
