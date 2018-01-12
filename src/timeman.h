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

#include <iostream>

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
  const int dynCon[COLOR_NB][6][8] =
  {
      {            //  White
  //Eval: <-30 -20 -10  <0 >=0 >10 >20 >30
          {-20,-24,-28, -5,  1,  1,  0, -2},  //        Diff < -20
          { -5, -3,  0,  0,  0, -2,  2,  1},  // -20 <= Diff < -10
          { -2,  4,  3,  0,  1, -1, -4,  1},  // -10 <= Diff <   0
          { -2,  0,  2,  2, -2, -4, -1,  2},  //   0 <= Diff <  10
          {  1,  3,  0, -2,  0,  0,  0,  5},  //  10 <= Diff <  20
          {  0,  2,  0, -2,  0,  0,  7, 12}   //  20 <= Diff
      },
      {            //  Black
          {-27,-24, -6,  0,  1,  0,  3, -2},  //        Diff < -20
          { -7, -1,  0,  0, -2,  0,  0, -4},  // -20 <= Diff < -10
          {  3,  0, -2,  0, -1,  0,  2, -1},  // -10 <= Diff <   0
          { -2,  1,  1,  0,  0,  1, -2,  0},  //   0 <= Diff <  10
          {  0, -2, -2, -1,  1,  3,  0,  5},  //  10 <= Diff <  20
          { -2, -3,  0, -1, 16, 14, 13, 11}   //  20 <= Diff
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
          sync_cout << "info string diff " << (scores.at(NumMoves) - scores.at(0)) << " eval "
                    << scores.at(NumMoves) << " ret " << ret << sync_endl;
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
