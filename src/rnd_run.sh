#!/bin/bash

# rnd_run.sh - add more randomised net args and run (mix of old and new nets)


for i in 1 2 3
do

   # No jobs running allows wait to be used
   if [[ "$(jobs | wc -l)" != 0 ]] ; then
      echo "There are already jobs running in this shell"
      exit 1
   fi


   python3 rndadj.py

   out="none"
#  read -p "run cutechess tests? " ans
ans=y
   case "$ans" in
      [Yy])
         out=$(. ./net_run.sh 2>net_run.err)
         ;;
      *)
         exit 2
   esac
   echo "tests started, out=<$out>"

   # Wait for tests to finish and then update db

   wait

   # use $out ...
   #python3 results.py

done

