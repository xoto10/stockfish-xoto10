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
  unsigned NumMoves = 10;
  int MinDiffInc = 30;
  int MinDiffDec = -30;
  int MinEvalInc = 30;
  int MinEvalDec = -30;
  int ContemptInc = 5;
  int ContemptDec = -5;
  int MaxContemptInc = 30;
  int MaxContemptDec = -30;
TUNE(SetRange(0,50), MinDiffInc, SetRange(-50,0), MinDiffDec);
TUNE(SetRange(-50,50), MinEvalInc, MinEvalDec);
TUNE(SetRange(0,30), ContemptInc, SetRange(-30,0), ContemptDec);


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
  int get_dynamic_contempt()
  {
      if (scores.size() >= NumMoves+1)
      {
          int ret, diff = (scores.at(NumMoves) - scores.at(0));

          if (diff >= MinDiffInc && scores.at(NumMoves) >= MinEvalInc && Options["Contempt"] >= 0)
              ret = std::min(ContemptInc, MaxContemptInc-Options["Contempt"]);
          else if (diff <= MinDiffDec && scores.at(NumMoves) <= MinEvalDec)
              ret = std::max(ContemptDec, MaxContemptDec-Options["Contempt"]);
          else
              ret = 0;
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
