/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2018 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

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

#ifndef TIMEMAN_H_INCLUDED
#define TIMEMAN_H_INCLUDED

#include "misc.h"
#include "search.h"
#include "thread.h"
#include "uci.h"

/// The TimeManagement class computes the optimal time to think depending on
/// the maximum available time, the game move number and other parameters.

class TimeManagement {
public:

  // Constants for dynamic contempt
  const unsigned NumMoves = 10;
/*
  const int MinDiffInc[COLOR_NB] = {20,19};
  const int MinEvalInc[COLOR_NB] = {29,-2};
  const int ContemptInc[COLOR_NB] = {16,14};
  const int MaxContemptInc = 72;

  const int MinDiffDec[COLOR_NB] = {-21,-23};
  const int MinEvalDec[COLOR_NB] = {-12,-19};
  const int ContemptDec[COLOR_NB] = {-30,-27};
  const int MaxContemptDec = -72;
*/
  const int dynCon[COLOR_NB][6][8] =
  {
      {            //  White
  //Eval: <-30 -20 -10  <0 >=0  >10 >20 >30
          {-24,-24, -6,  0,  0,  0,  0,  0},  //        Diff < -20
          { -6,  0,  0,  0,  0,  0,  0,  0},  // -20 <= Diff < -10
          {  0,  0,  0,  0,  0,  0,  0,  0},  // -10 <= Diff <   0
          {  0,  0,  0,  0,  0,  0,  0,  0},  //   0 <= Diff <  10
          {  0,  0,  0,  0,  0,  0,  0,  4},  //  10 <= Diff <  20
          {  0,  0,  0,  0,  0,  0,  4, 13}   //  20 <= Diff
      },
      {            //  Black
          {-24,-24, -6,  0,  0,  0,  0,  0},  //        Diff < -20
          { -6,  0,  0,  0,  0,  0,  0,  0},  // -20 <= Diff < -10
          {  0,  0,  0,  0,  0,  0,  0,  0},  // -10 <= Diff <   0
          {  0,  0,  0,  0,  0,  0,  0,  0},  //   0 <= Diff <  10
          {  0,  0,  0,  0,  0,  0,  0,  4},  //  10 <= Diff <  20
          {  0,  0,  0,  4, 13, 13, 13, 13}   //  20 <= Diff
      }
  };  // Us, (diff+30)/10-1, (sc+40)/10-1 to index into here

  void init(Search::LimitsType& limits, Color us, int ply);
  void init_scores()
  {
      scores.clear();
      lastVal = VALUE_NONE;
      saveVal = VALUE_NONE;
  }
  void update_scores()
  {
      if (saveVal != VALUE_NONE)
      {
          Value avg2 = ((lastVal==VALUE_NONE ? saveVal : lastVal) + saveVal) / 2;
          scores.push_back(avg2);
          if (scores.size() > NumMoves+1)
              scores.pop_front();
          lastVal = saveVal;
      }
  }
  int get_dynamic_contempt(Color Us)
  {
      if (scores.size() >= NumMoves+1)
      {
          int d = std::min( std::max((30 + scores.at(NumMoves) - scores.at(0)) / 10 - 1, 0), 5);
          int e = std::min( std::max((40 + scores.at(NumMoves)) / 10 - 1, 0), 7);
          int ret = dynCon[Us][d][e];
          return(ret);
      }
      else
          return(0);
  }
  int optimum() const { return optimumTime; }
  int maximum() const { return maximumTime; }
  int elapsed() const { return int(Search::Limits.npmsec ? Threads.nodes_searched() : now() - startTime); }

  int64_t availableNodes; // When in 'nodes as time' mode

  Value saveVal;
  Value lastVal;
  std::deque<Value> scores = {};  // last 11 values for average of lastVal and saveVal

private:
  TimePoint startTime;
  int optimumTime;
  int maximumTime;
};

extern TimeManagement Time;

#endif // #ifndef TIMEMAN_H_INCLUDED
