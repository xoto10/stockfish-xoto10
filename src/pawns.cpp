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

#include <algorithm>
#include <cassert>

#include "bitboard.h"
#include "pawns.h"
#include "position.h"
#include "thread.h"

namespace {

  #define V Value
  #define S(mg, eg) make_score(mg, eg)

  // Pawn penalties
  constexpr Score Backward = S( 9, 24);
  constexpr Score Doubled  = S(11, 56);
  constexpr Score Isolated = S( 5, 15);

  // Connected pawn bonus
  constexpr int Connected[RANK_NB] = { 0, 13, 17, 24, 59, 96, 171 };

  // Strength of pawn shelter for our king by [distance from edge][rank].
  // RANK_1 = 0 is used for files where we have no pawn, or pawn is behind our king.
  constexpr Score ShelterStrength[int(FILE_NB) / 2][RANK_NB] = {
    { S( -6, 0), S( 81, 0), S( 93, 0), S( 58, 0), S( 39, 0), S( 18, 0), S(  25, 0) },
    { S(-43, 0), S( 61, 0), S( 35, 0), S(-49, 0), S(-29, 0), S(-11, 0), S( -63, 0) },
    { S(-10, 0), S( 75, 0), S( 23, 0), S( -2, 0), S( 32, 0), S(  3, 0), S( -45, 0) },
    { S(-39, 0), S(-13, 0), S(-29, 0), S(-52, 0), S(-48, 0), S(-67, 0), S(-166, 0) }
  };

  // Danger of enemy pawns moving toward our king by [distance from edge][rank].
  // RANK_1 = 0 is used for files where the enemy has no pawn, or their pawn
  // is behind our king.
  constexpr Score UnblockedStorm[int(FILE_NB) / 2][RANK_NB] = {
    { S( 89, 44), S(107, 53), S(123, 61), S(93,46), S(57,28), S( 45, 22), S( 51, 25) },
    { S( 44, 22), S(-18, -9), S(123, 61), S(46,23), S(39,19), S( -7, -3), S( 23, 11) },
    { S(  4,  2), S( 52, 26), S(162, 81), S(37,18), S( 7, 3), S(-14, -7), S( -2, -1) },
    { S(-10, -5), S(-14, -7), S( 90, 45), S(15, 7), S( 2, 1), S( -7, -3), S(-16, -8) }
  };

  #undef S
  #undef V

  template<Color Us>
  Score evaluate(const Position& pos, Pawns::Entry* e) {

    constexpr Color     Them = (Us == WHITE ? BLACK : WHITE);
    constexpr Direction Up   = (Us == WHITE ? NORTH : SOUTH);

    Bitboard b, neighbours, stoppers, doubled, support, phalanx;
    Bitboard lever, leverPush;
    Square s;
    bool opposed, backward;
    Score score = SCORE_ZERO;
    const Square* pl = pos.squares<PAWN>(Us);

    Bitboard ourPawns   = pos.pieces(  Us, PAWN);
    Bitboard theirPawns = pos.pieces(Them, PAWN);

    e->passedPawns[Us] = e->pawnAttacksSpan[Us] = e->weakUnopposed[Us] = 0;
    e->kingSquares[Us]   = SQ_NONE;
    e->pawnAttacks[Us]   = pawn_attacks_bb<Us>(ourPawns);

    // Loop through all pawns of the current color and score each pawn
    while ((s = *pl++) != SQ_NONE)
    {
        assert(pos.piece_on(s) == make_piece(Us, PAWN));

        File f = file_of(s);
        Rank r = relative_rank(Us, s);

        e->pawnAttacksSpan[Us] |= pawn_attack_span(Us, s);

        // Flag the pawn
        opposed    = theirPawns & forward_file_bb(Us, s);
        stoppers   = theirPawns & passed_pawn_span(Us, s);
        lever      = theirPawns & PawnAttacks[Us][s];
        leverPush  = theirPawns & PawnAttacks[Us][s + Up];
        doubled    = ourPawns   & (s - Up);
        neighbours = ourPawns   & adjacent_files_bb(f);
        phalanx    = neighbours & rank_bb(s);
        support    = neighbours & rank_bb(s - Up);

        // A pawn is backward when it is behind all pawns of the same color
        // on the adjacent files and cannot be safely advanced.
        backward =  !(ourPawns & pawn_attack_span(Them, s + Up))
                  && (stoppers & (leverPush | (s + Up)));

        // Passed pawns will be properly scored in evaluation because we need
        // full attack info to evaluate them. Include also not passed pawns
        // which could become passed after one or two pawn pushes when are
        // not attacked more times than defended.
        if (   !(stoppers ^ lever ^ leverPush)
            && (support || !more_than_one(lever))
            && popcount(phalanx) >= popcount(leverPush))
            e->passedPawns[Us] |= s;

        else if (stoppers == square_bb(s + Up) && r >= RANK_5)
        {
            b = shift<Up>(support) & ~theirPawns;
            while (b)
                if (!more_than_one(theirPawns & PawnAttacks[Us][pop_lsb(&b)]))
                    e->passedPawns[Us] |= s;
        }

        // Score this pawn
        if (support | phalanx)
        {
            int v = (phalanx ? 3 : 2) * Connected[r];
            v = 17 * popcount(support) + (v >> (opposed + 1));
            score += make_score(v, v * (r - 2) / 4);
        }
        else if (!neighbours)
            score -= Isolated, e->weakUnopposed[Us] += !opposed;

        else if (backward)
            score -= Backward, e->weakUnopposed[Us] += !opposed;

        if (doubled && !support)
            score -= Doubled;
    }

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
void Entry::evaluate_shelter(const Position& pos, Square ksq, Score& shelter) {

  constexpr Color     Them = (Us == WHITE ? BLACK : WHITE);
  constexpr Direction Down = (Us == WHITE ? SOUTH : NORTH);
  constexpr Bitboard BlockSquares =  (Rank1BB | Rank2BB | Rank7BB | Rank8BB)
                                   & (FileABB | FileHBB);

  Bitboard b = pos.pieces(PAWN) & ~forward_ranks_bb(Them, ksq);
  Bitboard ourPawns = b & pos.pieces(Us);
  Bitboard theirPawns = b & pos.pieces(Them);

  Score safety = (shift<Down>(theirPawns) & BlockSquares & ksq)
                 ? make_score(374, 0) : make_score(5, 0);

  File center = clamp(file_of(ksq), FILE_B, FILE_G);
  for (File f = File(center - 1); f <= File(center + 1); ++f)
  {
      b = ourPawns & file_bb(f);
      Rank ourRank = b ? relative_rank(Us, backmost_sq(Us, b)) : RANK_1;

      b = theirPawns & file_bb(f);
      Rank theirRank = b ? relative_rank(Us, frontmost_sq(Them, b)) : RANK_1;

      int d = std::min(f, ~f);
      safety += ShelterStrength[d][ourRank];
      if (ourRank && (ourRank == theirRank - 1))
          safety -= make_score(82, 82) * (theirRank == RANK_3);
      else
          safety -= UnblockedStorm[d][theirRank];
  }

  if (mg_value(safety) > mg_value(shelter))
      shelter = safety;
}


/// Entry::do_king_safety() calculates a bonus for king safety. It is called only
/// when king square changes, which is about 20% of total king_safety() calls.

template<Color Us>
Score Entry::do_king_safety(const Position& pos) {

  Square ksq = pos.square<KING>(Us);
  kingSquares[Us] = ksq;
  castlingRights[Us] = pos.castling_rights(Us);

  Bitboard pawns = pos.pieces(Us, PAWN);
  int minPawnDist = pawns ? 8 : 0;

  if (pawns & PseudoAttacks[KING][ksq])
      minPawnDist = 1;

  else while (pawns)
      minPawnDist = std::min(minPawnDist, distance(ksq, pop_lsb(&pawns)));

  Score shelter = make_score(-VALUE_INFINITE, Value(0));
  evaluate_shelter<Us>(pos, ksq, shelter);

  // If we can castle use the bonus after the castling if it is bigger
  if (pos.can_castle(Us | KING_SIDE))
      evaluate_shelter<Us>(pos, relative_square(Us, SQ_G1), shelter);

  if (pos.can_castle(Us | QUEEN_SIDE))
      evaluate_shelter<Us>(pos, relative_square(Us, SQ_C1), shelter);

  return shelter - make_score(0, 16 * minPawnDist);
}

// Explicit template instantiation
template Score Entry::do_king_safety<WHITE>(const Position& pos);
template Score Entry::do_king_safety<BLACK>(const Position& pos);

} // namespace Pawns
