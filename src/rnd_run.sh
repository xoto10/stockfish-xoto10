#!/bin/bash

# rnd_run.sh - add more randomised net args and run (mix of old and new nets)


# No jobs running allows wait to be used
if [[ "$(jobs | wc -l)" != 0 ]] ; then
   echo "There are already jobs running in this shell"
   exit 1
fi

# Clear tmp files
\rm net_list.wk[12]
touch net_list.wk[12]


# Loop a few times
for i in 1 2 3 4 5 6 7 8 9 10
do

   if [[ -r "rnd_run.stop" ]] ; then
      break
   fi

   touch net_run.sh
   out1=$(python3 rndadj.py 2>&1)
   echo "rnds generated, out1=<
$out1>"


#  read -p "run cutechess tests? " ans
out2=
ans=y
   case "$ans" in
      [Yy])
         out2=$(. ./net_run.sh)
         ;;
      *)
         exit 2
   esac

   # Wait for tests to finish and then update db (use out2?)

   echo "out2=<
$out2>"
   wait; sleep 1
   wait; sleep 1
   wait; sleep 1

   # get run list from net_run.sh and parse results
   nns=$(grep nn- net_run.sh | sed 's/.*nn-/nn-/
                                    s/ [^ ]*$//')
   echo "nns=<$nns>" nns2 $nns
   python3 results.py $nns

done

