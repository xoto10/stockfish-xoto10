
# results.py - takes list of nn-000x arguments and extracts results from related log files

import sys
import re
import shutil


def read_list():
   #os.rename(src,dst)
   with open('net_list.dat') as fi:
#     with open('net_list.wk2', 'w') as fo:
         for line in fi:
#              print("list line found: "+line)
               nums = re.sub('.*:','',line)
#              print("nums "+nums)
               (a,b,c) = nums.split(',')
               w,l,d = int(a),int(b),int(c)
#              print("nums %d %d %d" % (w,l,d))
               line = re.sub('\n$', '', line)
               if w+l+d > 0:
                  print("%-110s%5.3f %5.3f" % (line, (w+d*0.5)/(w+l+d), float(d)/(w+l+d)))
               else:
                  print(line)



read_list()


