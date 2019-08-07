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

enum Param    { RM, SB2, SB3, SB5, DC1, DC2, FE1, NM1, NM2, NM3, NM4, NM5, NM6, NM7, NM8, PB2,
                EX6, LM3, FB, LM13, LM14, LM15, LM16, PARAM_NB };
enum ParamDbl { FE3, PARAM_DBL_NB };
enum ParamTyp { ParMin, ParMax, ParM, ParC, PAR_TYP_NB };

const int Params[][PAR_TYP_NB] = { {   531,  785,   12,   371 },  // RM
                                   {   -26,   13, -582,    40 },  // SB2
                                   {    18,   31, -167,    34 },  // SB3
                                   {   117,  165,  738,    66 },  // SB5
                                   {    74,  103, -357,   115 },  // DC1
                                   {   149,  218, -841,   251 },  // DC2

                                   {   282,  391, 1557,   222 },  // FE1
                                   { 19628,26938, -383, 29454 },  // NM1
                                   {    29,   34,   94,    25 },  // NM2
                                   {   210,  362,    9,   114 },  // NM3
                                   {   792,  890,   -9,  1001 },  // NM4
                                   {    63,   76, -132,    83 },  // NM5

                                   {   111,  272,   -9,   387 },  // NM6
                                   {     2,    3,  -16,     4 },  // NM7
                                   {    12,   15,   83,     7 },  // NM8
                                   {   127,  250,   -8,   371 },  // PB2
                                   {    30,   44, -119,    47 },  // EX6
                                   {   208,  288,-1357,   370 },  // LM3

                                   {   127,  179,  701,    91 },  // FB
                                   {  -242,    6,   13,  -404 },  // LM13
                                   {  -307,   22,   18,  -460 },  // LM14
                                   {  -155, -100,  684,  -181 },  // LM15
                                   {  -414,  211,  -36,   640 }   // LM16
                                 };
const int PDivs[] = {   1, 256, 256, 256, 256, 256,
                      256,   1, 256,   1,   1, 256,
                        1, 256, 256,   1, 256, 256,
                      256,   1,   1, 256,   1 };

const double ParamDbls[][PAR_TYP_NB] = { { 460.7, 804.7, 19.7, 221.3 }
                                    };

template <Param p>
int vary(int x)
{
  return clamp((Params[p][ParM] * x / PDivs[p] + Params[p][ParC]), Params[p][ParMin], Params[p][ParMax]);
}

template <ParamDbl p>
double vary(double x)
{
  return clamp(ParamDbls[p][ParM] * x + ParamDbls[p][ParC], ParamDbls[p][ParMin], ParamDbls[p][ParMax]);
}


void init();
void clear();

} // namespace Search

#endif // #ifndef SEARCH_H_INCLUDED
