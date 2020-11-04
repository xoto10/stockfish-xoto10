#/bin/python3

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
      a = int(random.random() * 3)
      b = int(random.random() * 3)
      c = a + b - 3
      s += str(c) + ","

   return s[0:-1]


random.seed()

lastNetId = read_net_list()

print("%05d:OUT163:%s" % (lastNetId+1, randout1()))

