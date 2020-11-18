#!/usr/bin/python3

# stats.py - output winrate and drawrate for net list

#python3 stats.py | grep '5.....6..$'


import re
import sys


def read_list(cond):
   with open('net_list.dat') as fi:
      for line in fi:
            nums = re.sub('.*:','',line)
            (a,b,c) = nums.split(',')
            w,l,d = int(a),int(b),int(c)
            line = re.sub('\n$', '', line)
            if w+l+d > 0:
               if (   cond == 'a'
                   or (cond == 'g' and (w+d*0.5)/(w+l+d) >= 0.5 and float(d)/(w+l+d) >= 0.6)):
                  print("%-103s%5.3f %5.3f" % (line, (w+d*0.5)/(w+l+d), float(d)/(w+l+d)))
            else:
               print(line)



cond = 'a'
if len(sys.argv) > 1 and sys.argv[1] == 'g':
   cond = 'g'
read_list(cond)


