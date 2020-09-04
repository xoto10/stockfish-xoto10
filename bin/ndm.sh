#!/bin/bash


awk '/ndm/ { print; n=1; }
/info depth/ { sub(/ nodes .*/, ""); info = $0; }
/ucinew/     { if (n) print info, "\n"; n=0; }
END          { if (n) print info, "\n"; n=0; }' ${1:-sfnew.log}
