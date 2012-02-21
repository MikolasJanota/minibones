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

class LitBitSet {
public:
  inline void add(Lit l);
  inline bool remove(Lit l);
  inline bool get(Lit l) const;
  inline void clear();
  ostream& print(ostream& out);
private:
  vector<bool> set;
};

inline void LitBitSet::add(Lit l) {
  const size_t li = literal_index(l);
  if (li >= set.size()) set.resize (li+1, false);
  set[li]=true;
}

inline bool LitBitSet::remove(Lit l) {
  const size_t li = literal_index(l);
  if (li >= set.size()) return false;
  set[li]=false;
  return true;
}

inline bool LitBitSet::get(Lit l) const {
  const size_t li = literal_index(l);
  if (li >= set.size()) return false;
  return set[li];
}

inline void LitBitSet::clear()  { set.clear(); }
#endif /* LITBITSET_HH_21039 */
