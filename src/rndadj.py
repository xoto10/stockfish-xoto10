#/bin/python3

import os
import sys
import random


#global numActive


def read_net_list():
   global numActive
   netId = -1
   with open('net_list.dat') as fi:
      for l in fi:
         (netId, adjType, adjs, active, scores) = l.split(':')
         if (active):
            numActive = numActive + 1
            with open('net_run.sh', 'a') as fo:
               fo.write("nohup ./cute_1+0.07adj 500 nnrnd1a master 1 1 %s nn-%05d &\n" % (adjs, int(netId)))

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

def new_adj(n, netId):
   for j in range(n):

      netId = netId + 1
      adjs = randout1()

      with open('net_list.dat', 'a') as f:
         f.write("%05d:OUT163:%s::0,0,0\n" % (netId, adjs))

      with open('net_run.sh', 'a') as f:
         f.write("nohup ./cute_1+0.07adj 1000 nnrnd1a master 3 1 %s nn-%05d &\n" % (adjs, netId))


# seed
random.seed()
if (len(sys.argv) > 1):
   random.seed(sys.argv[1])
   print("%s used as extra seed" % (sys.argv[1]))

# header
os.rename('net_run.sh', 'net_run_old.sh')
with open('net_run.sh', 'w') as f:
   f.write("#!/bin/bash\n\n")

# new net(s)
numActive = 0
netId = read_net_list()
new_adj(3-numActive, netId)
print("Active nets: " + str(numActive) + " new nets: 3")


