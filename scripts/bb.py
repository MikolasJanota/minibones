#!/usr/bin/python

import string, sys, re 

# read a set of literals from a file, 
# assuming literals start on a line with [prefix]
def rbb(prefix,f):
  s=set()
  ln=0
  for line in f: 
     ln=ln+1
     if line.startswith(prefix):
       for l in line[len(prefix):].split():
          s.add(int(l))
  return s

