/*
 * File:  LitBitSet.cc
 * Author:  mikolas
 * Created on:  Mon, Feb 20, 2012 11:47:06 AM
 * Copyright (C) 2012, Mikolas Janota
 */
#include "LitBitSet.hh"

ostream& LitBitSet::print(ostream& out) {
  for (size_t index = 0; index < set.size(); ++index) {
    if (!set[index]) continue;
    if (index) out << " ";
    out << index2literal(index);
  }
  return out;
}
