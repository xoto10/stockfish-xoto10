
# stats.py - output winrate and drawrate for net list

#python3 stats.py | grep '5.....6..$'


import re


def read_list():
   with open('net_list.dat') as fi:
      for line in fi:
            nums = re.sub('.*:','',line)
            (a,b,c) = nums.split(',')
            w,l,d = int(a),int(b),int(c)
            line = re.sub('\n$', '', line)
            if w+l+d > 0:
               print("%-110s%5.3f %5.3f" % (line, (w+d*0.5)/(w+l+d), float(d)/(w+l+d)))
            else:
               print(line)



read_list()


