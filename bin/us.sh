#!/bin/bash


grep -a -e ' Total ' -e ' them ' sfnew.log |
    awk '{ us += $6; them += $8; }
         END { print "us " us " them " them; }'

