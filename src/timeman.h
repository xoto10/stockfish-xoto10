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

const int DiffMin = 20;       // score difference over last 10 moves
const int MinScore = 20;
const int BaseContempt = 0;
const int ExtraContempt = 30;
const int HighContempt = 30;

/// The TimeManagement class computes the optimal time to think depending on
/// the maximum available time, the game move number and other parameters.

class TimeManagement {
public:
  void init(Search::LimitsType& limits, Color us, int ply);
  void initOppMoves()
  {
      lastMove = "";
      lastPonder = "";
      oppMoves = 0;
      oppDiffs = 0;
      lastVal = VALUE_NONE;
  }
  void update_scores(std::string move, std::string pond)
  {
      Value avg2 = ((lastVal==VALUE_NONE ? saveVal : lastVal) + saveVal) / 2;
      scores.push_back(avg2);
      if (scores.size() > 11)
          scores.pop_front();
      lastVal = saveVal;
      lastMove = move;
      lastPonder = pond;
  }
  int get_dynamic_contempt(int contempt)
  {
      if (scores.size() >= 11)
      {
          int diff = (scores.at(10) - scores.at(0));
          sync_cout << "info string s0 " << scores.at(0) << " s10 " << scores.at(10) << " diff " << diff << sync_endl;
          return(diff >= DiffMin && scores.at(10) >= MinScore && contempt >= 0
                 ? std::min(ExtraContempt, HighContempt-contempt) : BaseContempt);
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
  std::deque<Value> scores = {};  // last 11 values for lastVal2
  std::string lastMove;
  std::string lastPonder;
  int oppMoves;
  int oppDiffs;

private:
  TimePoint startTime;
  int optimumTime;
  int maximumTime;
};

extern TimeManagement Time;

#endif // #ifndef TIMEMAN_H_INCLUDED
