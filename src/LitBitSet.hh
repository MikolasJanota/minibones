/*
 * File:  LitBitSet.hh
 * Author:  mikolas
 * Created on:  Mon, Feb 20, 2012 11:47:02 AM
 * Copyright (C) 2012, Mikolas Janota
 */
#ifndef LITBITSET_HH_21039
#define LITBITSET_HH_21039
#include <vector>
#include "minisat_aux.hh"
using std::vector;

class LitBitSet;

class  const_infinite_LitBitSetIterator {
public:
  const_infinite_LitBitSetIterator(const LitBitSet& ls, size_t x);
  const_infinite_LitBitSetIterator(const const_infinite_LitBitSetIterator& mit) 
  : ls(mit.ls), i(mit.i) {}
  inline const_infinite_LitBitSetIterator& operator++();
  inline const Lit operator*() const;
  bool operator==(const const_infinite_LitBitSetIterator& rhs) { assert(&ls==&(rhs.ls)); return i==rhs.i;}
  bool operator!=(const const_infinite_LitBitSetIterator& rhs) { assert(&ls==&(rhs.ls)); return i!=rhs.i;}
private:
  const LitBitSet&            ls;
  size_t                      i;
};

class LitBitSet {
public:
  LitBitSet() : _size(0) {}
  inline bool                      add(Lit l);
  inline bool                      remove(Lit l);
  inline bool                      get(Lit l) const;
  inline void                      clear();
  size_t                           size() const {return _size;}
  ostream&                         print(ostream& out);
  const_infinite_LitBitSetIterator infinite_iterator() const {return const_infinite_LitBitSetIterator(*this, 0);}
  size_t                           physical_size () const {return set.size();}
private:
  size_t         _size;
  vector<bool>   set;
};

inline bool LitBitSet::add(Lit l) {
  const size_t li = literal_index(l);
  if (li >= set.size()) set.resize (li+1, false);
  if (set[li]) return false;
  set[li]=true;
  ++_size;
  return true;
}

inline bool LitBitSet::remove(Lit l) {
  const size_t li = literal_index(l);
  if (li >= set.size() || !set[li] ) return false;
  set[li]=false;
  --_size;
  return true;
}

inline bool LitBitSet::get(Lit l) const {
  const size_t li = literal_index(l);
  if (li >= set.size()) return false;
  return set[li];
}

inline void LitBitSet::clear()  { set.clear(); _size = 0; }

inline const_infinite_LitBitSetIterator& const_infinite_LitBitSetIterator::operator++() {
  if (!ls.size()) return *this;
  assert(i < ls.physical_size());
  assert(0 <= i);
  do { i=(i+1) % ls.physical_size(); }
  while (i<2 || !ls.get(index2literal(i)));
  assert (ls.get(index2literal(i)));
  return *this;
}

inline const Lit const_infinite_LitBitSetIterator::operator*() const {
  const Lit return_value = index2literal(i);
  assert(ls.get(return_value));
  return return_value;
}
#endif /* LITBITSET_HH_21039 */
