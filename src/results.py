#!/usr/bin/python3

# results.py - takes list of nn-000x arguments and extracts results from related log files


import sys
import re
import shutil


def get_wld(a):
   w = l = d = -1
   with open('cute_1+0.07adj_'+a+'.out') as fi:
      for line in fi:
         if line[0:8] == 'Score of':
#           print(line)
            wd = line.split(' ')
            if len(wd) > 9:
               #(s,o,b,v,m,w,f,l,e,d,etc) =
               w = wd[5]
               l = wd[7]
               d = wd[9]
#           print("win "+w+"loss "+l+"draw "+d)
   if w != -1:
      print("win "+w+" loss "+l+" draw "+d)
   return (w,l,d)


def update_list(a,w,l,d):
   #os.rename(src,dst)
   with open('net_list.wk1') as fi:
      with open('net_list.wk2', 'w') as fo:
         for line in fi:
            if re.search('^'+a[3:], line):
               print("list line found: "+line)
               nums = re.sub('.*:','',line)
               print("nums "+nums)
               (ww,ll,dd) = nums.split(',')
               print("nums %s %s %s" % (ww,ll,dd))
               base = re.sub(':[0-9,-]*\n$','',line)
               fo.write("%s:%d,%d,%d\n" % (base, int(ww)+int(w), int(ll)+int(l), int(dd)+int(d)))
            else:
               fo.write(line)
   shutil.copy2('net_list.wk2', 'net_list.wk1')



#Score of nnrnd1a vs master: 9 - 4 - 10  [0.609] 23
shutil.copy2('net_list.dat', 'net_list.wk1')
# join -t : -a 1 net_list.dat net_list.wk2 -o 1.0,2.1,2.2 >net_list.wk1
for a in sys.argv[1:]:
   print("Processing %s ..." % (a))
   (w,l,d) = get_wld(a)
   update_list(a,w,l,d)



