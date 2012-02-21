#!/usr/bin/python

import string, sys, re 
from bb import *

asz=len(sys.argv)-1

if ((asz&1)==1): 
  print 'even no. of args expected'

args=sys.argv[1:]

bss=[]
for i in range(asz/2):
    f = open(args[i*2+1], 'r')
    bs=rbb(args[i*2], f)
    bss.append(bs)

wrong=False
for i in range(len(bss)-1):
  for j in range(i+1,len(bss)):
    print 'comparing',args[i*2+1],args[j*2+1]
    if bss[i]!=bss[j]:
      print 'differ: ',args[i*2+1],args[j*2+1]
      print bss[i]-bss[j]
      print bss[j]-bss[i]
      wrong=True

exit(100 if wrong else 0)
