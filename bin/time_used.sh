#!/bin/bash


LOG=${1:-sfnew.log}



awk 'BEGIN               { tinfo="none";
                         }
     /^>> ucinewgame/    { work=1;
                           if (tinfo != "none" && scinfo ~ /cp 0/) {
#                            print pinfo;
                             if (length(pinfo) < 10)
                               print "black", tinfo;
                             else
                               print "white", tinfo;
#                            print scinfo;
                           }
#                          print "."; print $0;
                         }
     /^>> position /     { if (work) {
                             sub(/.* - [01] [01] moves/, "");
                             pinfo=$0;
                             #if (length($0) > 1) print;
                           }
                         }
     /^>> go wtime/      { tinfo=$0; }
     /^<< info depth 1 / { if (work) {
                             work=0;
                           }
                         }
     /^<< info .* cp /   {  if (1) {
                             sub(/.* cp /, "cp ");
                             sub(/ nodes .*/, "");
                             scinfo=$0;
                           }
                         }
    ' $LOG |
awk '/white /  { us=us+$5; them=them+$7; n=n+1; }
     /black /  { them=them+$5; us=us+$7; n=n+1; }
     END       { printf "us: %d them: %d n: %d\n", us, them, n;
                 printf "us: %d them: %d\n", us/n, them/n; }'



#>> ucinewgame
#>> position fen rnbqkbnr/pp2ppp1/2p4p/3p4/P7/2P2N2/1P1PPPPP/RNBQKB1R w KQkq - 0 1 moves d2d4
#>> go wtime 443 btime 859 winc 50 binc 50
#<< info depth 1 seldepth 1 multipv 1 score cp 46 nodes 29 nps 14500 tbhits 0 time 2 pv c8f5


