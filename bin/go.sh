#!/bin/bash

# go

SD=../src/stockfish-xoto10/src
IN=fensh1.txt
FENS=gofens.tmp
GO=go.tmp
FENS2=gofens2.tmp
GOJOIN=gojoin.tmp
GOJOIN2=gojoin2.tmp
OUT=go.out

function h
{
    echo
    echo "$1:"
    head -10 "$1"
}


# Remove : chars if present
sed 's/:/ /g' <$IN >$FENS
h $FENS

# stockfish go depth 13 and add line numbers
$SD/stockfish_mastergo1a bench 16 1 ${1:-13} $FENS depth 2>&1 |
    awk '/info depth/  { last=$0; }
         /bestmove/    { print last; }' |
    sed 's/ nodes.*//' |
    sed 's/.* fen //' |
    sed 's/seldepth.*score //' |
    nl \
    >$GO 2>&1
h $GO

# Prepend with line number and 6char fen
sed 's/^\(......\)\(.*\)/\1 \1\2/' $IN | nl >$FENS2
h $FENS2

# Join fens and sf output
join $FENS2 $GO >$GOJOIN
h $GOJOIN

# Remove the 6char fen fields
awk '{
       if ($2 == $6)
       { 
         $2=""; $6="";
         $8 = sprintf("% 04d", $8);
         print;
       }
     }
    ' $GOJOIN | sed 's/  / /g' >$GOJOIN2
h $GOJOIN2

# if creating new list remove old score
if [[ "$2" = new ]] ; then
    sed 's/-*[0-9][0-9][0-9] cp //' $GOJOIN2 >$OUT
else
    cp $GOJOIN2 $OUT
fi
h $OUT


exit 0


# Res is from w pov, score added here is from side to move
# Fen                                          Res Sc
# 8/p3kp1p/1p4p1/8/8/4P1P1/4K2P/4N3:b:-:-:0:36 0.5 090
# 2r2nk1/pbqrbpp1/1pn1p2p/2ppP3/P2P4/1PPBNNBP/4QPP1/1R1R2K1:b:-:-:8:23 0.0 070

# 
