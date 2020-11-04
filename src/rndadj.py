#/bin/python3

import sys
import random


def read_net_list():
   netId = -1
   with open('net_list.dat') as f:
      for l in f:
         (netId, adjType, adjs) = l.split(':')
   return int(netId)

def randout1():
   s = ""
   for i in range(32):
      c = 0
      if (random.random() < 0.5):
         a = int(random.random() * 4)
         b = int(random.random() * 4)
         c = a + b - 3
      s += str(c) + ","

   return s[0:-1]


random.seed()
if (len(sys.argv) > 1):
   random.seed(sys.argv[1])
   print("%s used as extra seed" % (sys.argv[1]))

with open('net_run.sh', 'a') as f:
   f.write("#!/bin/bash\n\n")

for j in range(3):

   netId = read_net_list() + 1
   adj = randout1()

   with open('net_list.dat', 'a') as f:
      f.write("%05d:OUT163:%s\n" % (netId, adj))

   with open('net_run.sh', 'a') as f:
      f.write("./cute_1+0.07 500 nnrnd1a1 master 1 1 %s &\n" % (adj))


