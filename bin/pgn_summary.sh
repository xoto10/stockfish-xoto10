#!/bin/bash


PGN=${1:-cc_22.pgn}

#grep -F -e '[White' -e '[Black' -e '[Result' -e '[ECO' -e '[Opening' -e '[Variation' $PGN >summ.txt

#awk '/\[White /    { gsub(/\[White "/, "");    sub(/"\]/, " ");      out = sprintf("%-15s", $0); }
#     /\[Black /    { gsub(/\[Black "/, "");    sub(/"\]/, " ");      out = out sprintf("%-15s", $0); }
#     /\[Result /   { gsub(/\[Result "/, "");   sub(/"\]/, "      "); out = out substr($0, 1, 8); }
#     /\[ECO /      { gsub(/\[ECO "/, "");      sub(/"\]/, " ");      out = out $0; }
#     /\[Opening /  { gsub(/\[Opening /, "");   sub(/\]/, " ");       out = out sprintf("%-30s", $0); }
#     /\[Variation /{ gsub(/\[Variation /, ""); sub(/\]/, "");        out = out $0; print out; out = ""; }' \
#   $PGN

#BEGIN    { last = ""; }

cat $PGN |
    grep -e '^[1-3]\. ' -e ' [1-3]\. ' -e '^\[Result ' -e '^\[White ' -e '^\[Black ' |
    sed 's/{[^{]*}//g'                                 |
    awk '
         /^\[Res/     { result = $0; }
         /^\[Whi/     { white  = $2; gsub(/["\]]/, "", white); }
         /^\[Bla/     { black  = $2; gsub(/["\]]/, "", black); }
         /^1\.[ \.]/  { if (last) print white " vs " black " " last result;
                        last = "";
                      }
         /^[^[]/      { last = last $0; }
         END          { print white " vs " black " " last result; }'          |
    sort                                               |
    awk '{ lin = "";
           for(i = 1 ; i <= NF ; i++)
           {
               w = 5;
               if (i == 1 || i == 3) w = 12;
               if (i == 2) w == 3;
               lin = lin sprintf("%-*s", w, $i);
           }
           if (substr(lin,1,15) != move1) print "";
           move1 = substr(lin,1,15);
           print lin; }'                               |
    sed -e 's/Result"/Result "/' -e 's/Result "1\/2-1\/2"/Result            /'



