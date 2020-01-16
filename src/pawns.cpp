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

#include <algorithm>
#include <cassert>

#include "bitboard.h"
#include "pawns.h"
#include "position.h"
#include "thread.h"

namespace {

inline Range vary30(int c) { return Range(c-30, c+30); }

int C1=2, C2=2, C3=4, C4=2, C5=2, C6=4, I1=0, B1=5, B2=5, KE1=0, KE2=16;

  #define V Value
  #define S(mg, eg) make_score(mg, eg)

  // Pawn penalties
            Score BW            = S( 9, 24);
            Score BS            = S( 3,  6);
            Score BU            = S(13, 27);
            Score BLS           = S(82, 82);
            Score DB       [2]  = { S( 5, 28), S( 2, 10) };
            Score DF       [2]  = { S(11, 56), S( 3, 18) };
            Score IS            = S( 5, 15);
            Score IU            = S(13, 27);
            Score PIS           = S( 3,  6);
            Score SU            = S( 3,  6);
            Score WL       [2]  = { S( 0, 56), S( 0, 10) };

  // Connected pawn bonus
            int CO1      [RANK_NB] = { 0, 7, 8, 12, 29, 48, 86 };
            int CO2      [RANK_NB] = { 0, 7, 8, 12, 29, 48, 86 };
            int SP      = 21;

  // Strength of pawn shelter for our king by [distance from edge][rank].
  // RANK_1 = 0 is used for files where we have no pawn, or pawn is behind our king.
            Value SS             [int(FILE_NB) / 2][RANK_NB] = {
    { V( -6), V( 81), V( 93), V( 58), V( 39), V( 18), V(  25) },
    { V(-43), V( 61), V( 35), V(-49), V(-29), V(-11), V( -63) },
    { V(-10), V( 75), V( 23), V( -2), V( 32), V(  3), V( -45) },
    { V(-39), V(-13), V(-29), V(-52), V(-48), V(-67), V(-166) }
  };

  // Danger of enemy pawns moving toward our king by [distance from edge][rank].
  // RANK_1 = 0 is used for files where the enemy has no pawn, or their pawn
  // is behind our king. Note that UnblockedStorm[0][1-2] accommodate opponent pawn
  // on edge, likely blocked by our king.
            Value UB            [int(FILE_NB) / 2][RANK_NB] = {
    { V( 85), V(-289), V(-166), V(97), V(50), V( 45), V( 50) },
    { V( 46), V( -25), V( 122), V(45), V(37), V(-10), V( 20) },
    { V( -6), V(  51), V( 168), V(34), V(-2), V(-22), V(-14) },
    { V(-15), V( -11), V( 101), V( 4), V(11), V(-15), V(-29) }
  };

TUNE(SetRange(vary30), C1, C2, C3, C4, C5, C6, I1, B1, B2, KE1, KE2);
TUNE(BW,BS,BU,BLS,DB,DF,IS,IU,PIS,SU,WL,CO1,CO2,SP,SS,UB);

  #undef S
  #undef V

  template<Color Us>
  Score evaluate(const Position& pos, Pawns::Entry* e) {

    constexpr Color     Them = (Us == WHITE ? BLACK : WHITE);
    constexpr Direction Up   = pawn_push(Us);

    Bitboard neighbours, stoppers, support, phalanx, opposed;
    Bitboard lever, leverPush, blocked;
    Square s;
    bool backward, passed, doubledFr, doubledBk, supporting, onIsland = false;
    Score score = SCORE_ZERO;
    int islands = -1;
    const Square* pl = pos.squares<PAWN>(Us);

    Bitboard ourPawns   = pos.pieces(  Us, PAWN);
    Bitboard theirPawns = pos.pieces(Them, PAWN);

    Bitboard doubleAttackThem = pawn_double_attacks_bb<Them>(theirPawns);

    e->passedPawns[Us] = 0;
    e->kingSquares[Us] = SQ_NONE;
    e->pawnAttacks[Us] = e->pawnAttacksSpan[Us] = pawn_attacks_bb<Us>(ourPawns);

    // Loop through all pawns of the current color and score each pawn
    while ((s = *pl++) != SQ_NONE)
    {
        assert(pos.piece_on(s) == make_piece(Us, PAWN));

        Rank r = relative_rank(Us, s);

        // Flag the pawn
        opposed    = theirPawns & forward_file_bb(Us, s);
        blocked    = theirPawns & (s + Up);
        stoppers   = theirPawns & passed_pawn_span(Us, s);
        lever      = theirPawns & PawnAttacks[Us][s];
        leverPush  = theirPawns & PawnAttacks[Us][s + Up];
        doubledFr  = ourPawns   & (s - Up);
        doubledBk  = ourPawns   & (s + Up);
        neighbours = ourPawns   & adjacent_files_bb(s);
        phalanx    = neighbours & rank_bb(s);
        support    = neighbours & rank_bb(s - Up);
        supporting = neighbours & rank_bb(s + Up);

        // A pawn is backward when it is behind all pawns of the same color on
        // the adjacent files and cannot safely advance.
        backward =  !(neighbours & forward_ranks_bb(Them, s + Up))
                  && (leverPush | blocked);

        // Compute additional span if pawn is not backward nor blocked
        if (!backward && !blocked)
            e->pawnAttacksSpan[Us] |= pawn_attack_span(Us, s);

        // A pawn is passed if one of the three following conditions is true:
        // (a) there is no stoppers except some levers
        // (b) the only stoppers are the leverPush, but we outnumber them
        // (c) there is only one front stopper which can be levered.
        passed =   !(stoppers ^ lever)
                || (   !(stoppers ^ leverPush)
                    && popcount(phalanx) >= popcount(leverPush))
                || (   stoppers == blocked && r >= RANK_5
                    && (shift<Up>(support) & ~(theirPawns | doubleAttackThem)));

        // Passed pawns will be properly scored later in evaluation when we have
        // full attack info.
        if (passed)
            e->passedPawns[Us] |= s;

        // Score this pawn
        if (support)
        {
            int v =  CO1      [r] * (C1 + bool(phalanx) - bool(opposed))
                   + SP      * popcount(support);

            score += make_score(v, v * (r - C2) / C3);
        }

        else if (phalanx)
        {
            int v =  CO2      [r] * (C4 + bool(phalanx) - bool(opposed));

            score += make_score(v, v * (r - C5) / C6);
        }

        else if (!neighbours)
            score -=   IS
                     + IU           * !opposed;

        else if (backward)
            score -=   BW
                     + BU            * !opposed
                     - BS             * supporting;

        else if (supporting)
            score +=   SU        ;

        score -=   WL       [bool(support)] * more_than_one(lever);

        score -=   DF       [bool(support)] * doubledFr
                 + DB       [bool(support)] * doubledBk;
    }

    // Count pawn islands
    for (File f=FILE_A; f<=FILE_H; ++f)
    {
        if (ourPawns & file_bb(f))
            islands += !onIsland,
            onIsland = true;
        else
            onIsland = false;
    }
    if (islands > I1)
        score -= PIS        * islands;

    return score;
  }

} // namespace

namespace Pawns {

/// Pawns::probe() looks up the current position's pawns configuration in
/// the pawns hash table. It returns a pointer to the Entry if the position
/// is found. Otherwise a new Entry is computed and stored there, so we don't
/// have to recompute all when the same pawns configuration occurs again.

Entry* probe(const Position& pos) {

  Key key = pos.pawn_key();
  Entry* e = pos.this_thread()->pawnsTable[key];

  if (e->key == key)
      return e;

  e->key = key;
  e->scores[WHITE] = evaluate<WHITE>(pos, e);
  e->scores[BLACK] = evaluate<BLACK>(pos, e);

  return e;
}


/// Entry::evaluate_shelter() calculates the shelter bonus and the storm
/// penalty for a king, looking at the king file and the two closest files.

template<Color Us>
Score Entry::evaluate_shelter(const Position& pos, Square ksq) {

  constexpr Color Them = (Us == WHITE ? BLACK : WHITE);

  Bitboard b = pos.pieces(PAWN) & ~forward_ranks_bb(Them, ksq);
  Bitboard ourPawns = b & pos.pieces(Us);
  Bitboard theirPawns = b & pos.pieces(Them);

  Score bonus = make_score(B1, B2);

  File center = clamp(file_of(ksq), FILE_B, FILE_G);
  for (File f = File(center - 1); f <= File(center + 1); ++f)
  {
      b = ourPawns & file_bb(f);
      int ourRank = b ? relative_rank(Us, frontmost_sq(Them, b)) : 0;

      b = theirPawns & file_bb(f);
      int theirRank = b ? relative_rank(Us, frontmost_sq(Them, b)) : 0;

      File d = map_to_queenside(f);
      bonus += make_score(SS             [d][ourRank], 0);

      if (ourRank && (ourRank == theirRank - 1))
          bonus -= BLS          * int(theirRank == RANK_3);
      else
          bonus -= make_score(UB            [d][theirRank], 0);
  }

  return bonus;
}


/// Entry::do_king_safety() calculates a bonus for king safety. It is called only
/// when king square changes, which is about 20% of total king_safety() calls.

template<Color Us>
Score Entry::do_king_safety(const Position& pos) {

  Square ksq = pos.square<KING>(Us);
  kingSquares[Us] = ksq;
  castlingRights[Us] = pos.castling_rights(Us);
  auto compare = [](Score a, Score b) { return mg_value(a) < mg_value(b); };

  Score shelter = evaluate_shelter<Us>(pos, ksq);

  // If we can castle use the bonus after castling if it is bigger

  if (pos.can_castle(Us & KING_SIDE))
      shelter = std::max(shelter, evaluate_shelter<Us>(pos, relative_square(Us, SQ_G1)), compare);

  if (pos.can_castle(Us & QUEEN_SIDE))
      shelter = std::max(shelter, evaluate_shelter<Us>(pos, relative_square(Us, SQ_C1)), compare);

  // In endgame we like to bring our king near our closest pawn
  Bitboard pawns = pos.pieces(Us, PAWN);
  int minPawnDist = pawns ? 8 : 0;

  if (pawns & PseudoAttacks[KING][ksq])
      minPawnDist = 1;
  else while (pawns)
      minPawnDist = std::min(minPawnDist, distance(ksq, pop_lsb(&pawns)));

  return shelter - make_score(KE1, KE2 * minPawnDist);
}

// Explicit template instantiation
template Score Entry::do_king_safety<WHITE>(const Position& pos);
template Score Entry::do_king_safety<BLACK>(const Position& pos);

} // namespace Pawns
