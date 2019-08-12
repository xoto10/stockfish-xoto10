/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2019 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include <vector>

#include "misc.h"
#include "movepick.h"
#include "types.h"

class Position;

namespace Search {

/// Threshold used for countermoves based pruning
constexpr int CounterMovePruneThreshold = 0;


/// Stack struct keeps track of the information we need to remember from nodes
/// shallower and deeper in the tree during the search. Each search thread has
/// its own array of Stack objects, indexed by the current ply.

struct Stack {
  Move* pv;
  PieceToHistory* continuationHistory;
  int ply;
  Move currentMove;
  Move excludedMove;
  Move killers[2];
  Value staticEval;
  int statScore;
  int moveCount;
};


/// RootMove struct is used for moves at the root of the tree. For each root move
/// we store a score and a PV (really a refutation in the case of moves which
/// fail low). Score is normally set at -VALUE_INFINITE for all non-pv moves.

struct RootMove {

  explicit RootMove(Move m) : pv(1, m) {}
  bool extract_ponder_from_tt(Position& pos);
  bool operator==(const Move& m) const { return pv[0] == m; }
  bool operator<(const RootMove& m) const { // Sort in descending order
    return m.score != score ? m.score < score
                            : m.previousScore < previousScore;
  }

  Value score = -VALUE_INFINITE;
  Value previousScore = -VALUE_INFINITE;
  int selDepth = 0;
  int tbRank = 0;
  Value tbScore;
  std::vector<Move> pv;
};

typedef std::vector<RootMove> RootMoves;


/// LimitsType struct stores information sent by GUI about available time to
/// search the current move, maximum depth/time, or if we are in analysis mode.

struct LimitsType {

  LimitsType() { // Init explicitly due to broken value-initialization of non POD in MSVC
    time[WHITE] = time[BLACK] = inc[WHITE] = inc[BLACK] = npmsec = movetime = TimePoint(0);
    movestogo = depth = mate = perft = infinite = 0;
    nodes = 0;
  }

  bool use_time_management() const {
    return !(mate | movetime | depth | nodes | perft | infinite);
  }

  std::vector<Move> searchmoves;
  TimePoint time[COLOR_NB], inc[COLOR_NB], npmsec, movetime, startTime;
  int movestogo, depth, mate, perft, infinite;
  int64_t nodes;
};

extern LimitsType Limits;


/// Param and vary() implement a simple linear slope with limits enforced via clamp(). This allows
/// parameters that would otherwise be constant to vary according to another variable, eg. depth.

enum Param    { RM, SB2, SB5, DC1, FE1, NM1, NM2, NM4, NM5, NM7, NM8, EX6, LM3, FB, LM15, PARAM_NB };
enum ParamDbl { FE3, PARAM_DBL_NB };
enum ParamTyp { ParMin, ParMax, ParM, ParC, PAR_TYP_NB };

const int Params[][PAR_TYP_NB] = { {   542,  798,   12,   374 },  // RM
                                   {   -27,   14, -548,    40 },  // SB2
                                   {   116,  169,  755,    67 },  // SB5
                                   {    73,  101, -348,   118 },  // DC1
                                   {   277,  403, 1616,   226 },  // FE1

                                   { 19870,26708, -396, 31075 },  // NM1
                                   {    30,   34,   93,    26 },  // NM2
                                   {   797,  868,   -9,   991 },  // NM4
                                   {    62,   74, -126,    85 },  // NM5
                                   {     2,    3,  -16,     4 },  // NM7

                                   {    12,   16,   81,     7 },  // NM8
                                   {    31,   41, -115,    47 },  // EX6
                                   {   202,  262,-1346,   374 },  // LM3
                                   {   131,  168,  699,    89 },  // FB
                                   {  -154, -106,  678,  -186 },  // LM15
                                 };
const int PDivs[] = {   1, 256, 256, 256, 256,
                        1, 256,   1, 256, 256,
                      256, 256, 256, 256, 256 };

template <Param p>
int vary(int x)
{
  return clamp((Params[p][ParM] * x / PDivs[p] + Params[p][ParC]), Params[p][ParMin], Params[p][ParMax]);
}


void init();
void clear();

} // namespace Search

#endif // #ifndef SEARCH_H_INCLUDED
