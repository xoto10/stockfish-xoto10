#!/bin/bash


awk '/ndm/ { ndm = $0; n=1; }
/info depth/ { sub(/nodes .*/, ""); sub(/seldepth.* score /, "score "); sub(/<< info/,""); sub(/ [a-z]*bound/, ""); info = $0; }
/ucinew/     { if (n && info !~ / cp 0 /) { print ndm info; } n=0; ndm = ""; }
END          { if (n && info !~ / cp 0 /) { print ndm info; n=0; } }' ${1:-sfnew.log}
