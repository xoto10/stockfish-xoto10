/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2020 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

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

#include <cassert>
#include <cstring>   // For std::memset

#include "material.h"
#include "thread.h"

using namespace std;

namespace {

int QO1=1438, QO2=40, QO3=38, QO4=32, QO5=255, QO6=-62, QO7=0, QO8=104, QO9=4, QO10=0, QO11=-26;
int QO12=-2, QO13=47, QO14=105, QO15=-208, QO16=-189, QO17=24, QO18=117, QO19=133, QO20=-134, QO21=-6;

int QT1=36, QT2=9, QT3=63, QT4=59, QT5=65, QT6=42, QT7=46, QT8=39, QT9=24, QT10=-24;
int QT11=97, QT12=100, QT13=-42, QT14=137, QT15=268;

  // Polynomial material imbalance parameters

  int QuadraticOurs[][PIECE_TYPE_NB] = {
    //            OUR PIECES
    // pair pawn knight bishop rook queen
    { QO1                               }, // Bishop pair
    { QO2,  QO3                         }, // Pawn
    { QO4,  QO5, QO6                    }, // Knight      OUR PIECES
    { QO7,  QO8, QO9,  QO10             }, // Bishop
    {QO11, QO12,QO13,  QO14,  QO15      }, // Rook
    {QO16, QO17,QO18,  QO19,  QO20,QO21 }  // Queen
  };

  int QuadraticTheirs[][PIECE_TYPE_NB] = {
    //           THEIR PIECES
    // pair pawn knight bishop rook queen
    {   0                               }, // Bishop pair
    { QT1,    0                         }, // Pawn
    { QT2,  QT3,   0                    }, // Knight      OUR PIECES
    { QT4,  QT5, QT6,     0             }, // Bishop
    { QT7,  QT8, QT9,  QT10,    0       }, // Rook
    {QT11, QT12,QT13,  QT14, QT15,    0 }  // Queen
  };

  // Update QuadraticOurs from QO vars
  void updateQO() {
    QuadraticOurs[0][0] = QO1;
    QuadraticOurs[1][0] = QO2;
    QuadraticOurs[1][1] = QO3;
    QuadraticOurs[2][0] = QO4;
    QuadraticOurs[2][1] = QO5;
    QuadraticOurs[2][2] = QO6;
    QuadraticOurs[3][0] = QO7;
    QuadraticOurs[3][1] = QO8;
    QuadraticOurs[3][2] = QO9;
    QuadraticOurs[3][3] = QO10;
    QuadraticOurs[4][0] = QO11;
    QuadraticOurs[4][1] = QO12;
    QuadraticOurs[4][2] = QO13;
    QuadraticOurs[4][3] = QO14;
    QuadraticOurs[4][4] = QO15;
    QuadraticOurs[5][0] = QO16;
    QuadraticOurs[5][1] = QO17;
    QuadraticOurs[5][2] = QO18;
    QuadraticOurs[5][3] = QO19;
    QuadraticOurs[5][4] = QO20;
    QuadraticOurs[5][5] = QO21;
  }

  // Update QuadraticTheirs from QO vars
  void updateQT() {
    QuadraticTheirs[1][0] = QT1;
    QuadraticTheirs[2][0] = QT2;
    QuadraticTheirs[2][1] = QT3;
    QuadraticTheirs[3][0] = QT4;
    QuadraticTheirs[3][1] = QT5;
    QuadraticTheirs[3][2] = QT6;
    QuadraticTheirs[4][0] = QT7;
    QuadraticTheirs[4][1] = QT8;
    QuadraticTheirs[4][2] = QT9;
    QuadraticTheirs[4][3] = QT10;
    QuadraticTheirs[5][0] = QT11;
    QuadraticTheirs[5][1] = QT12;
    QuadraticTheirs[5][2] = QT13;
    QuadraticTheirs[5][3] = QT14;
    QuadraticTheirs[5][4] = QT15;
  }

Range vary20(int c) {
  return (abs(c) < 20) ? Range(c-20, c+20) : c < 0 ? Range(c * 2, 0) : Range(0, c * 2);
}

TUNE(SetRange(vary20), QO1, QO2, QO3, QO4, QO5, QO6, QO8, QO7, QO9, QO10, QO11,
     QO13, QO14, QO15, QO16, QO18, QO19, QO20, QO12, QO17, QO21, updateQO);

TUNE(SetRange(vary20), QT1, QT3, QT4, QT5, QT6, QT7, QT8, QT2, QT9, QT10,
     QT11, QT12, QT13, QT14, QT15, updateQT);

  // Endgame evaluation and scaling functions are accessed directly and not through
  // the function maps because they correspond to more than one material hash key.
  Endgame<KXK>    EvaluateKXK[] = { Endgame<KXK>(WHITE),    Endgame<KXK>(BLACK) };

  Endgame<KBPsK>  ScaleKBPsK[]  = { Endgame<KBPsK>(WHITE),  Endgame<KBPsK>(BLACK) };
  Endgame<KQKRPs> ScaleKQKRPs[] = { Endgame<KQKRPs>(WHITE), Endgame<KQKRPs>(BLACK) };
  Endgame<KPsK>   ScaleKPsK[]   = { Endgame<KPsK>(WHITE),   Endgame<KPsK>(BLACK) };
  Endgame<KPKP>   ScaleKPKP[]   = { Endgame<KPKP>(WHITE),   Endgame<KPKP>(BLACK) };

  // Helper used to detect a given material distribution
  bool is_KXK(const Position& pos, Color us) {
    return  !more_than_one(pos.pieces(~us))
          && pos.non_pawn_material(us) >= RookValueMg;
  }

  bool is_KBPsK(const Position& pos, Color us) {
    return   pos.non_pawn_material(us) == BishopValueMg
          && pos.count<PAWN  >(us) >= 1;
  }

  bool is_KQKRPs(const Position& pos, Color us) {
    return  !pos.count<PAWN>(us)
          && pos.non_pawn_material(us) == QueenValueMg
          && pos.count<ROOK>(~us) == 1
          && pos.count<PAWN>(~us) >= 1;
  }

  /// imbalance() calculates the imbalance by comparing the piece count of each
  /// piece type for both colors.
  template<Color Us>
  int imbalance(const int pieceCount[][PIECE_TYPE_NB]) {

    constexpr Color Them = ~Us;

    int bonus = 0;

    // Second-degree polynomial material imbalance, by Tord Romstad
    for (int pt1 = NO_PIECE_TYPE; pt1 <= QUEEN; ++pt1)
    {
        if (!pieceCount[Us][pt1])
            continue;

        int v = 0;

        for (int pt2 = NO_PIECE_TYPE; pt2 <= pt1; ++pt2)
            v +=  QuadraticOurs[pt1][pt2] * pieceCount[Us][pt2]
                + QuadraticTheirs[pt1][pt2] * pieceCount[Them][pt2];

        bonus += pieceCount[Us][pt1] * v;
    }

    return bonus;
  }

} // namespace

namespace Material {

/// Material::probe() looks up the current position's material configuration in
/// the material hash table. It returns a pointer to the Entry if the position
/// is found. Otherwise a new Entry is computed and stored there, so we don't
/// have to recompute all when the same material configuration occurs again.

Entry* probe(const Position& pos) {

  Key key = pos.material_key();
  Entry* e = pos.this_thread()->materialTable[key];

  if (e->key == key)
      return e;

  std::memset(e, 0, sizeof(Entry));
  e->key = key;
  e->factor[WHITE] = e->factor[BLACK] = (uint8_t)SCALE_FACTOR_NORMAL;

  Value npm_w = pos.non_pawn_material(WHITE);
  Value npm_b = pos.non_pawn_material(BLACK);
  Value npm   = Utility::clamp(npm_w + npm_b, EndgameLimit, MidgameLimit);

  // Map total non-pawn material into [PHASE_ENDGAME, PHASE_MIDGAME]
  e->gamePhase = Phase(((npm - EndgameLimit) * PHASE_MIDGAME) / (MidgameLimit - EndgameLimit));

  // Let's look if we have a specialized evaluation function for this particular
  // material configuration. Firstly we look for a fixed configuration one, then
  // for a generic one if the previous search failed.
  if ((e->evaluationFunction = Endgames::probe<Value>(key)) != nullptr)
      return e;

  for (Color c : { WHITE, BLACK })
      if (is_KXK(pos, c))
      {
          e->evaluationFunction = &EvaluateKXK[c];
          return e;
      }

  // OK, we didn't find any special evaluation function for the current material
  // configuration. Is there a suitable specialized scaling function?
  const auto* sf = Endgames::probe<ScaleFactor>(key);

  if (sf)
  {
      e->scalingFunction[sf->strongSide] = sf; // Only strong color assigned
      return e;
  }

  // We didn't find any specialized scaling function, so fall back on generic
  // ones that refer to more than one material distribution. Note that in this
  // case we don't return after setting the function.
  for (Color c : { WHITE, BLACK })
  {
    if (is_KBPsK(pos, c))
        e->scalingFunction[c] = &ScaleKBPsK[c];

    else if (is_KQKRPs(pos, c))
        e->scalingFunction[c] = &ScaleKQKRPs[c];
  }

  if (npm_w + npm_b == VALUE_ZERO && pos.pieces(PAWN)) // Only pawns on the board
  {
      if (!pos.count<PAWN>(BLACK))
      {
          assert(pos.count<PAWN>(WHITE) >= 2);

          e->scalingFunction[WHITE] = &ScaleKPsK[WHITE];
      }
      else if (!pos.count<PAWN>(WHITE))
      {
          assert(pos.count<PAWN>(BLACK) >= 2);

          e->scalingFunction[BLACK] = &ScaleKPsK[BLACK];
      }
      else if (pos.count<PAWN>(WHITE) == 1 && pos.count<PAWN>(BLACK) == 1)
      {
          // This is a special case because we set scaling functions
          // for both colors instead of only one.
          e->scalingFunction[WHITE] = &ScaleKPKP[WHITE];
          e->scalingFunction[BLACK] = &ScaleKPKP[BLACK];
      }
  }

  // Zero or just one pawn makes it difficult to win, even with a small material
  // advantage. This catches some trivial draws like KK, KBK and KNK and gives a
  // drawish scale factor for cases such as KRKBP and KmmKm (except for KBBKN).
  if (!pos.count<PAWN>(WHITE) && npm_w - npm_b <= BishopValueMg)
      e->factor[WHITE] = uint8_t(npm_w <  RookValueMg   ? SCALE_FACTOR_DRAW :
                                 npm_b <= BishopValueMg ? 4 : 14);

  if (!pos.count<PAWN>(BLACK) && npm_b - npm_w <= BishopValueMg)
      e->factor[BLACK] = uint8_t(npm_b <  RookValueMg   ? SCALE_FACTOR_DRAW :
                                 npm_w <= BishopValueMg ? 4 : 14);

  // Evaluate the material imbalance. We use PIECE_TYPE_NONE as a place holder
  // for the bishop pair "extended piece", which allows us to be more flexible
  // in defining bishop pair bonuses.
  const int pieceCount[COLOR_NB][PIECE_TYPE_NB] = {
  { pos.count<BISHOP>(WHITE) > 1, pos.count<PAWN>(WHITE), pos.count<KNIGHT>(WHITE),
    pos.count<BISHOP>(WHITE)    , pos.count<ROOK>(WHITE), pos.count<QUEEN >(WHITE) },
  { pos.count<BISHOP>(BLACK) > 1, pos.count<PAWN>(BLACK), pos.count<KNIGHT>(BLACK),
    pos.count<BISHOP>(BLACK)    , pos.count<ROOK>(BLACK), pos.count<QUEEN >(BLACK) } };

  e->value = int16_t((imbalance<WHITE>(pieceCount) - imbalance<BLACK>(pieceCount)) / 16);
  return e;
}

} // namespace Material
