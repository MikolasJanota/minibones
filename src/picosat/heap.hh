//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        heap.hh
 *
 * Description: Abstract heap, with extended priority queue interface.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2010, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _HEAP_H
#define _HEAP_H 1

#include <vector>

using namespace std;

#define fixindex(i) _heap[i]->index() = i;


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: Abstract heap, with extended priority queue interface.
 *
 * Purpose: To be used in standard heap/priority queue apps.
\*----------------------------------------------------------------------------*/
//jpms:ec

template <typename T, typename U>
class heap {

public:

  heap(vector<T>& ref) {
    _heap.resize(ref.size());
    copy(ref.begin(), ref.end(), _heap.begin()); build_heap(); fixindexes();
  }

  T maximum() { return _heap[1]; }   // Return element 1; element 0 is *unused*

  T extract_max() {                 // Return element 1 and remove it from heap
    T tmax = _heap[1];
    heap_swap(1,heap_size());
    _heap.pop_back(); siftdown(1);
    return tmax;
  }

  void update_key(T elem, U nv) {
    U cv = _heap[elem->index()]->key();
    if (cv > nv)      { decrease_key(elem, nv); }
    else if (cv < nv) { increase_key(elem, nv); }
  }

  void increase_key(T elem, U nv) {
    elem->key() = nv; siftup(elem->index()); }

  void decrease_key(T elem, U nv) {
    elem->key() = nv; siftdown(elem->index()); }

  bool is_heap() {
    bool heapok = true;
    for(int i=heap_size()/2; i>=1; --i) {
      int l = left(i); int r = right(i);
      if (_heap[i]->key() < _heap[l]->key() ||
	  r <= heap_size() && _heap[i]->key() < _heap[r]->key()) {
	heapok = false; break;
      }
    }
    return heapok;
  }

  void clear() { _heap.clear(); }

public:

  void dump(ostream& outs=cout) {
    typename vector<T>::iterator hpos = _heap.begin();
    typename vector<T>::iterator hend = _heap.end();
    if (hpos != hend) { hpos++; }
    for (; hpos != hend; ++hpos) { outs << **hpos << " ";  outs << endl; }
  }

  friend ostream & operator << (ostream& outs, heap<T,U>& h) {
    h.dump(outs); return outs;
  }

protected:

  void build_heap() { for(int i=heap_size()/2; i>=1; --i) { siftdown(i); } }

  void siftdown(int i) {
    assert(i <= heap_size());
    int l = left(i); int r = right(i); int largest = i;
    if (l <= heap_size() && _heap[l]->key() > _heap[i]->key()) {
      largest = l;
    }
    if (r <= heap_size() && _heap[r]->key() > _heap[largest]->key()) {
      largest = r;
    }
    if (largest != i) {
      heap_swap(i, largest); siftdown(largest);
    }
  }

  void siftup(int i) {
    int j = parent(i);
    while (j >= 1 && _heap[j]->key() < _heap[i]->key()) {
      heap_swap(i, j); i = j; j = parent(i);
    }
    assert(i>=1);
  }

protected:

  inline int parent(int i) { return i >> 1; }

  inline int left(int i) { return i << 1; }

  inline int right(int i) { return (i << 1) + 1; }

  inline int heap_size() { return _heap.size() - 1; }

  inline void heap_swap(int i, int j) {
    T tmp = _heap[i]; _heap[i] = _heap[j]; _heap[j] = tmp;
    _heap[i]->index() = i; _heap[j]->index() = j; }

  void fixindexes() { for(int i=1; i<=heap_size(); ++i) { fixindex(i); } }

protected:

  vector<T> _heap;

};

#endif /* _HEAP_H */

/*----------------------------------------------------------------------------*/
