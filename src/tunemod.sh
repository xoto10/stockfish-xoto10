#!/bin/bash


# gen:

if [[ "${2:-1}" = 1 ]] ; then
    awk '/constexpr .*=.*[0-9]+.*;$/
         /constexpr .*\[.*=/' \
        ${1:-evaluate.cpp} |
        grep -v -e LowRanks \
                -e TRank3BB |
        sed '
             s/ *constexpr *[a-zA-Z_0-9]* *//
             s/ *=.*$//
             s/\[.*$//
             s/  *$//
             s#\(.*\)#\1/@\1|/g#

             s/@WeakQueenProtection[^A-Za-z0-9_]/WQP/g
             s/@WeakQueen[^A-Za-z0-9_]/WQ/g
             s/@TrappedRook[^A-Za-z0-9_]/TR/g
             s/@ThreatBySafePawn[^A-Za-z0-9_]/TSP/g
             s/@ThreatByRook[^A-Za-z0-9_]/TBR/g
             s/@ThreatByPawnPush[^A-Za-z0-9_]/TPP/g
             s/@ThreatByMinor[^A-Za-z0-9_]/TBM/g
             s/@ThreatByKing[^A-Za-z0-9_]/TBK/g
             s/@SpaceThreshold[^A-Za-z0-9_]/ST/g
             s/@SliderOnQueen[^A-Za-z0-9_]/SOQ/g
             s/@RookSafeCheck[^A-Za-z0-9_]/RSC/g
             s/@RookOnQueenFile[^A-Za-z0-9_]/RQF/g
             s/@RookOnFile[^A-Za-z0-9_]/ROF/g
             s/@RestrictedPiece[^A-Za-z0-9_]/RP/g
             s/@QueenSafeCheck[^A-Za-z0-9_]/QSC/g
             s/@PawnlessFlank[^A-Za-z0-9_]/PLF/g
             s/@PassedRank[^A-Za-z0-9_]/PR/g
             s/@PassedFile[^A-Za-z0-9_]/PF/g
             s/@Outpost[^A-Za-z0-9_]/OP/g
             s/@MobilityBonus[^A-Za-z0-9_]/MB/g
             s/@MinorBehindPawn[^A-Za-z0-9_]/MBP/g
             s/@LongDiagonalBishop[^A-Za-z0-9_]/LDB/g
             s/@LazyThreshold[^A-Za-z0-9_]/LT/g
             s/@KnightSafeCheck[^A-Za-z0-9_]/KSC/g
             s/@KnightOnQueen[^A-Za-z0-9_]/KOQ/g
             s/@KingProtector[^A-Za-z0-9_]/KP/g
             s/@KingAttackWeights[^A-Za-z0-9_]/KAW/g
             s/@Hanging[^A-Za-z0-9_]/H/g
             s/@FlankAttacks[^A-Za-z0-9_]/FA/g
             s/@CorneredBishop[^A-Za-z0-9_]/CB/g
             s/@BishopSafeCheck[^A-Za-z0-9_]/BSC/g
             s/@BishopPawns[^A-Za-z0-9_]/BP/g

             s#/\([^@]\)#[^A-Za-z0-9_]/\1#
             s#^#             s/@#
             s///
            ' |
        sort -u -r
fi
#([^a-z0-9_])

# run:

if [[ "${2:-1}" = 2 ]] ; then

    # auto subs


    # evaluate.cpp

    # kd:
    sed 's/185 \* popcount/KD1 * popcount/
         s/148 \* popcount/KD2 * popcount/
         s/ 98 \* popcount/KD3 * popcount/
         s/ 69 \* kingAttacksCount/KD4 * kingAttacksCount/
         s/  3 \* kingFlankAttack /KD5 * kingFlankAttack /
         s/873 \* !pos.count</KD6 * !pos.count</
         s/100 \* bool(attack/KD7 * bool(attack/
         s/  6 \* mg_value(sc/KD8 * mg_value(sc/
         s/  4 \* kingFlankDe/KD9 * kingFlankDe/
         s/ 37;/KD10;/
         s/kingDanger > 100/kingDanger > KD11/

         s/KING>(c), s), 5);/KING>(c), s), PP1);/
         s/= 5 \* r - 13;/= PP2 * r - PP3;/
         s/blockSq) \* 19) \/ 4/blockSq) * PP4) \/ 4/
         s/blockSq) \*  2) \* w/blockSq) * PP5) * w/
         s/Squares                    ? 35 :/Squares                    ? PP6 :/
         s/eSquares & squaresToQueen) ? 20 :/eSquares \& squaresToQueen) ? PP7 :/
         s/eSquares & blockSq)        ?  9 :/eSquares \& blockSq)        ? PP8 :/
         s/                              0 ;/                             PP9 ;/
         s/k += 5;/k += PP10;/

         s/outflanking < 0/outflanking < CY1/
         s/   9 \* pe->passed_/  CY2 * pe->passed_/
         s/+ 11 \* pos.count<P/+ CY3 * pos.count<P/
         s/+  9 \* outflanking/+ CY4 * outflanking/
         s/+ 21 \* pawnsOnBoth/+ CY5 * pawnsOnBoth/
         s/+ 24 \* infiltratio/+ CY6 * infiltratio/
         s/+ 51 \* !pos.non_pa/+ CY7 * !pos.non_pa/
         s/- 43 \* almostUnwin/- CY8 * almostUnwin/
         s/-110 ;/-CY9 ;/
         s/min(complexity + 50/min(complexity + CY10/

             s/WeakQueenProtection\([^A-Za-z0-9_]\)/WQP\1/g
             s/WeakQueen\([^A-Za-z0-9_]\)/WQ\1/g
             s/TrappedRook\([^A-Za-z0-9_]\)/TR\1/g
             s/ThreatBySafePawn\([^A-Za-z0-9_]\)/TSP\1/g
             s/ThreatByRook\([^A-Za-z0-9_]\)/TBR\1/g
             s/ThreatByPawnPush\([^A-Za-z0-9_]\)/TPP\1/g
             s/ThreatByMinor\([^A-Za-z0-9_]\)/TBM\1/g
             s/ThreatByKing\([^A-Za-z0-9_]\)/TBK\1/g
             s/SpaceThreshold\([^A-Za-z0-9_]\)/ST\1/g
             s/SliderOnQueen\([^A-Za-z0-9_]\)/SOQ\1/g
             s/RookSafeCheck\([^A-Za-z0-9_]\)/RSC\1/g
             s/RookOnQueenFile\([^A-Za-z0-9_]\)/RQF\1/g
             s/RookOnFile\([^A-Za-z0-9_]\)/ROF\1/g
             s/RestrictedPiece\([^A-Za-z0-9_]\)/RP\1/g
             s/QueenSafeCheck\([^A-Za-z0-9_]\)/QSC\1/g
             s/PawnlessFlank\([^A-Za-z0-9_]\)/PLF\1/g
             s/PassedRank\([^A-Za-z0-9_]\)/PR\1/g
             s/PassedFile\([^A-Za-z0-9_]\)/PF\1/g
             s/Outpost\([^A-Za-z0-9_]\)/OP\1/g
             s/MobilityBonus\([^A-Za-z0-9_]\)/MB\1/g
             s/MinorBehindPawn\([^A-Za-z0-9_]\)/MBP\1/g
             s/LongDiagonalBishop\([^A-Za-z0-9_]\)/LDB\1/g
             s/LazyThreshold\([^A-Za-z0-9_]\)/LT\1/g
             s/KnightSafeCheck\([^A-Za-z0-9_]\)/KSC\1/g
             s/KnightOnQueen\([^A-Za-z0-9_]\)/KOQ\1/g
             s/KingProtector\([^A-Za-z0-9_]\)/KP\1/g
             s/KingAttackWeights\([^A-Za-z0-9_]\)/KAW\1/g
             s/Hanging\([^A-Za-z0-9_]\)/H\1/g
             s/FlankAttacks\([^A-Za-z0-9_]\)/FA\1/g
             s/CorneredBishop\([^A-Za-z0-9_]\)/CB\1/g
             s/BishopSafeCheck\([^A-Za-z0-9_]\)/BSC\1/g
             s/BishopPawns\([^A-Za-z0-9_]\)/BP\1/g

        ' evaluate.cpp >evaltune.cpp

    fi

-- evaluate.cpp
int KD1=185, KD2=148, KD3=98, KD4=69, KD5=3, KD6=873, KD7=100, KD8=6, KD9=4, KD10=37, KD11=100;
int PP1=5, PP2=5, PP3=13, PP4=19, PP5=2, PP6=35, PP7=20, PP8=9, PP9=0, PP10=5;
int CY1=0, CY2=9, CY3=11, CY4=9, CY5=21, CY6=24, CY7=51, CY8=43, CY9=110, CY10=50;
int SF1=22, SF2=36, SF3=2, SF4=36, SF5=7, SF6=12;

Range vary20(int c) { return Range(c-20, c+20); }

TUNE(SetDefaultRange, KD1, KD2, KD3, KD4, KD5, KD6, KD7, KD8, KD9, KD10, KD11);
TUNE(SetRange(vary20), PP1, PP2, PP3, PP4, PP5, PP6, PP7, PP8, PP9, PP10);
TUNE(SetDefaultRange, CY1, CY2, CY3, CY4, CY5, CY6, CY7, CY8, CY9, CY10);
TUNE(SetDefaultRange, SF1, SF2, SF3, SF4, SF5, SF6);

TUNE(SetDefaultRange, BP, BSC, CB, FA, H, KAW, KP, KOQ, KSC, LT, LDB, MBP, MB, OP, PF, PR, PLF, QSC);
TUNE(SetDefaultRange, RP, ROF, RQF, RSC, SOQ, ST, TBK, TBM, TPP, TBR, TSP, TR, WQP, WQ);

-- material.cpp
int QO1=1438, QO2=40, QO3=38, QO4=32, QO5=255, QO6=-62, QO7=0, QO8=104, QO9=4, QO10=0, QO11=-26;
int QO12=-2, QO13=47, QO14=105, QO15=-208, QO16=-189, QO17=24, QO18=117, QO19=133, QO20=-134, QO21=-6;

int QT1=36, QT2=9, QT3=63, QT4=59, QT5=65, QT6=42, QT7=46, QT8=39, QT9=24, QT10=-24;
int QT11=97, QT12=100, QT13=-42, QT14=137, QT15=268;

Range vary20(int c) { return Range(c-20, c+20); }

TUNE(SetDefaultRange, QO1, QO2, QO3, QO4, QO5, QO6, QO8, SetRange(vary20), QO7, QO9, QO10, QO11);
TUNE(SetDefaultRange, QO13, QO14, QO15, QO16, QO18, QO19, QO20, SetRange(vary20), QO12, QO17, QO21);

TUNE(SetDefaultRange, QT1, QT3, QT4, QT5, QT6, QT7, QT8, SetRange(vary20), QT2, QT9, QT10);
TUNE(SetDefaultRange, QT11, QT12, QT13, QT14, QT15);

--

exit 0

===============================


function substitute {
    if [[ "$#" == "2" ]] || [[ "$#" == "3" ]]; then
      # Input from stdin
      HAYSTACK="$(/bin/cat -)"
      NEEDLE="$1"
      NEEDLESUB="$2"
      REGEX_FLAGS="$3"
    else
      echo "Usage:   echo <HAYSTACK> | substitute <NEEDLE> <NEEDLESUB> [FLAGS]"
      echo "Example:   echo 'hello w' | substitute 'w' 'world'"
      echo "Example:   echo 'hello w' | substitute 'O' 'xxxxx' 'gi'"
    fi
    CLEAN_SED_STRING="$(echo "$NEEDLE" | sed 's:[]\[^$.*/&]:\\&:g')"
    CLEAN_SED_SUBSTRING="$(echo "$NEEDLESUB" | sed 's:[]\[^$.*/&]:\\&:g')"
    echo "$HAYSTACK" | sed "s/$CLEAN_SED_STRING/$CLEAN_SED_SUBSTRING/$REGEX_FLAGS"
}
#substitute '185 * popcount' 'KD1 * popcount'


# by hand

# kingDanger: 11 constants
# passed pawn constants 5/5/13/.../9/0/5
# complexity: 9 constants
# scalefactor: 22/36/...

# types.h - piece values etc
# material.cpp - quadratics


# General
Range vary20(int c) { return Range(c-20, c+20); }

# Files

========================

evaluate.cpp

    kingDanger +=        kingAttackersCount[Them] * kingAttackersWeight[Them]
                 + 185 * popcount(kingRing[Us] & weak)
                 + 148 * popcount(unsafeChecks)
                 +  98 * popcount(pos.blockers_for_king(Us))
                 +  69 * kingAttacksCount[Them]
                 +   3 * kingFlankAttack * kingFlankAttack / 8
                 +       mg_value(mobility[Them] - mobility[Us])
                 - 873 * !pos.count<QUEEN>(Them)
                 - 100 * bool(attackedBy[Us][KNIGHT] & attackedBy[Us][KING])
                 -   6 * mg_value(score) / 8
                 -   4 * kingFlankDefense
                 +  37;

    // Transform the kingDanger units into a Score, and subtract it from the evaluation
    if (kingDanger > 100)
        score -= make_score(kingDanger * kingDanger / 4096, kingDanger / 16);

----
int KD1=185, KD2=148, KD3=98, KD4=69, KD5=3, KD6=873, KD7=100, KD8=6, KD9=4, KD10=37, KD11=100;
    kingDanger +=        kingAttackersCount[Them] * kingAttackersWeight[Them]
                 + KD1 * popcount(kingRing[Us] & weak)
                 + KD2 * popcount(unsafeChecks)
                 + KD3 * popcount(pos.blockers_for_king(Us))
                 + KD4 * kingAttacksCount[Them]
                 + KD5 * kingFlankAttack * kingFlankAttack / 8
                 +       mg_value(mobility[Them] - mobility[Us])
                 - KD6 * !pos.count<QUEEN>(Them)
                 - KD7 * bool(attackedBy[Us][KNIGHT] & attackedBy[Us][KING])
                 - KD8 * mg_value(score) / 8
                 - KD9 * kingFlankDefense
                 + KD10;

    // Transform the kingDanger units into a Score, and subtract it from the evaluation
    if (kingDanger > KD11)
        score -= make_score(kingDanger * kingDanger / 4096, kingDanger / 16);

TUNE(SetDefaultRange, KD1, KD2, KD3, KD4, KD5, KD6, KD7, KD8, KD9, KD10, KD11);

====

    auto king_proximity = [&](Color c, Square s) {
      return std::min(distance(pos.square<KING>(c), s), 5);
    };

    Bitboard b, bb, squaresToQueen, unsafeSquares;
    Score score = SCORE_ZERO;

    b = pe->passed_pawns(Us);

    while (b)
    {
        Square s = pop_lsb(&b);

        assert(!(pos.pieces(Them, PAWN) & forward_file_bb(Us, s + Up)));

        int r = relative_rank(Us, s);

        Score bonus = PassedRank[r];

        if (r > RANK_3)
        {
            int w = 5 * r - 13;
            Square blockSq = s + Up;

            // Adjust bonus based on the kings proximity
            bonus += make_score(0, (  (king_proximity(Them, blockSq) * 19) / 4
                                     - king_proximity(Us,   blockSq) *  2) * w);

            // If blockSq is not the queening square then consider also a second push
            if (r != RANK_7)
                bonus -= make_score(0, king_proximity(Us, blockSq + Up) * w);

            // If the pawn is free to advance, then increase the bonus
            if (pos.empty(blockSq))
            {
                squaresToQueen = forward_file_bb(Us, s);
                unsafeSquares = passed_pawn_span(Us, s);

                bb = forward_file_bb(Them, s) & pos.pieces(ROOK, QUEEN);

                if (!(pos.pieces(Them) & bb))
                    unsafeSquares &= attackedBy[Them][ALL_PIECES];

                // If there are no enemy attacks on passed pawn span, assign a big bonus.
                // Otherwise assign a smaller bonus if the path to queen is not attacked
                // and even smaller bonus if it is attacked but block square is not.
                int k = !unsafeSquares                    ? 35 :
                        !(unsafeSquares & squaresToQueen) ? 20 :
                        !(unsafeSquares & blockSq)        ?  9 :
                                                             0 ;

                // Assign a larger bonus if the block square is defended
                if ((pos.pieces(Us) & bb) || (attackedBy[Us][ALL_PIECES] & blockSq))
                    k += 5;

----


int PP1=5, PP2=5, PP3=13, PP4=19, PP5=2, PP6=35, PP7=20, PP8=9, PP9=0, PP10=5;
    auto king_proximity = [&](Color c, Square s) {
      return std::min(distance(pos.square<KING>(c), s), PP1);
    };

    Bitboard b, bb, squaresToQueen, unsafeSquares;
    Score score = SCORE_ZERO;

    b = pe->passed_pawns(Us);

    while (b)
    {
        Square s = pop_lsb(&b);

        assert(!(pos.pieces(Them, PAWN) & forward_file_bb(Us, s + Up)));

        int r = relative_rank(Us, s);

        Score bonus = PassedRank[r];

        if (r > RANK_3)
        {
            int w = PP2 * r - PP3;
            Square blockSq = s + Up;

            // Adjust bonus based on the kings proximity
            bonus += make_score(0, (  (king_proximity(Them, blockSq) * PP4) / 4
                                     - king_proximity(Us,   blockSq) * PP5) * w);

            // If blockSq is not the queening square then consider also a second push
            if (r != RANK_7)
                bonus -= make_score(0, king_proximity(Us, blockSq + Up) * w);

            // If the pawn is free to advance, then increase the bonus
            if (pos.empty(blockSq))
            {
                squaresToQueen = forward_file_bb(Us, s);
                unsafeSquares = passed_pawn_span(Us, s);

                bb = forward_file_bb(Them, s) & pos.pieces(ROOK, QUEEN);

                if (!(pos.pieces(Them) & bb))
                    unsafeSquares &= attackedBy[Them][ALL_PIECES];

                // If there are no enemy attacks on passed pawn span, assign a big bonus.
                // Otherwise assign a smaller bonus if the path to queen is not attacked
                // and even smaller bonus if it is attacked but block square is not.
                int k = !unsafeSquares                    ? PP6 :
                        !(unsafeSquares & squaresToQueen) ? PP7 :
                        !(unsafeSquares & blockSq)        ? PP8 :
                                                            PP9 ;

                // Assign a larger bonus if the block square is defended
                if ((pos.pieces(Us) & bb) || (attackedBy[Us][ALL_PIECES] & blockSq))
                    k += PP10;

TUNE(SetRange(vary20), PP1, PP2, PP3, PP4, PP5, PP6, PP7, PP8, PP9, PP10);

====

    bool almostUnwinnable =   outflanking < 0
                           && !pawnsOnBothFlanks;

    bool infiltration = rank_of(pos.square<KING>(WHITE)) > RANK_4
                     || rank_of(pos.square<KING>(BLACK)) < RANK_5;

    // Compute the initiative bonus for the attacking side
    int complexity =   9 * pe->passed_count()
                    + 11 * pos.count<PAWN>()
                    +  9 * outflanking
                    + 21 * pawnsOnBothFlanks
                    + 24 * infiltration
                    + 51 * !pos.non_pawn_material()
                    - 43 * almostUnwinnable
                    -110 ;

    Value mg = mg_value(score);
    Value eg = eg_value(score);

    // Now apply the bonus: note that we find the attacking side by extracting the
    // sign of the midgame or endgame values, and that we carefully cap the bonus
    // so that the midgame and endgame scores do not change sign after the bonus.
    int u = ((mg > 0) - (mg < 0)) * std::max(std::min(complexity + 50, 0), -abs(mg));

----

int CY1=0, CY2=9, CY3=11, CY4=9, CY5=21, CY6=24, CY7=51, CY8=43, CY9=110, CY10=50;
    bool almostUnwinnable =   outflanking < CY1
                           && !pawnsOnBothFlanks;

    bool infiltration = rank_of(pos.square<KING>(WHITE)) > RANK_4
                     || rank_of(pos.square<KING>(BLACK)) < RANK_5;

    // Compute the initiative bonus for the attacking side
    int complexity =  CY2 * pe->passed_count()
                    + CY3 * pos.count<PAWN>()
                    + CY4 * outflanking
                    + CY5 * pawnsOnBothFlanks
                    + CY6 * infiltration
                    + CY7 * !pos.non_pawn_material()
                    - CY8 * almostUnwinnable
                    - CY9 ;

    Value mg = mg_value(score);
    Value eg = eg_value(score);

    // Now apply the bonus: note that we find the attacking side by extracting the
    // sign of the midgame or endgame values, and that we carefully cap the bonus
    // so that the midgame and endgame scores do not change sign after the bonus.
    int u = ((mg > 0) - (mg < 0)) * std::max(std::min(complexity + CY10, 0), -abs(mg));

TUNE(SetDefaultRange, CY1, CY2, CY3, CY4, CY5, CY6, CY7, CY8, CY9, CY10);

====

        if (   pos.opposite_bishops()
            && pos.non_pawn_material() == 2 * BishopValueMg)
            sf = 22 ;
        else
            sf = std::min(sf, 36 + (pos.opposite_bishops() ? 2 : 7) * pos.count<PAWN>(strongSide));

        sf = std::max(0, sf - (pos.rule50_count() - 12) / 4);

----

int SF1=22, SF2=36, SF3=2, SF4=36, SF5=7, SF6=12;
        if (   pos.opposite_bishops()
            && pos.non_pawn_material() == 2 * BishopValueMg)
            sf = SF1 ;
        else if (pos.opposite_bishops())
            sf = SF2 + SF3 * pos.count<PAWN>(strongSide)
        else
            sf = std::min(sf, SF4 + SF5 * pos.count<PAWN>(strongSide));

        sf = std::max(0, sf - (pos.rule50_count() - SF6) / 4);

TUNE(SetDefaultRange, SF1, SF2, SF3, SF4, SF5, SF6);

========================

types.h

  SCALE_FACTOR_NORMAL  = 64

  PawnValueMg   = 128,   PawnValueEg   = 213,
  KnightValueMg = 781,   KnightValueEg = 854,
  BishopValueMg = 825,   BishopValueEg = 915,
  RookValueMg   = 1276,  RookValueEg   = 1380,
  QueenValueMg  = 2538,  QueenValueEg  = 2682,
  Tempo = 28,

  MidgameLimit  = 15258, EndgameLimit  = 3915




----

int SFN=64, PVM=128, PVE=213, NVM=781, NVE=854, BVM=825, BVE=915, RVM=1276, RVE=1380;
int QVM=2538, QVE=2682, T=28, MGL=15258, EGL=3915;
  SCALE_FACTOR_NORMAL  = SFN,

  PawnValueMg   = PVM,   PawnValueEg   = PVE,
  KnightValueMg = NVM,   KnightValueEg = NVE,
  BishopValueMg = BVM,   BishopValueEg = BVE,
  RookValueMg   = RVM,   RookValueEg   = RVE,
  QueenValueMg  = QVM,   QueenValueEg  = QVE,
  Tempo = T,

  MidgameLimit  = MGL, EndgameLimit  = EGL

TUNE(SetDefaultRange, SFN, PVM, PVE, NVM, NVE, BVM, BVE, RVM, RVE);
TUNE(QVM, QVE, T, MGL, EGL);


========================

material.cpp


  constexpr int QuadraticOurs[][PIECE_TYPE_NB] = {
    //            OUR PIECES
    // pair pawn knight bishop rook queen
    {1438                               }, // Bishop pair
    {  40,   38                         }, // Pawn
    {  32,  255, -62                    }, // Knight      OUR PIECES
    {   0,  104,   4,    0              }, // Bishop
    { -26,   -2,  47,   105,  -208      }, // Rook
    {-189,   24, 117,   133,  -134, -6  }  // Queen
  };

  constexpr int QuadraticTheirs[][PIECE_TYPE_NB] = {
    //           THEIR PIECES
    // pair pawn knight bishop rook queen
    {   0                               }, // Bishop pair
    {  36,    0                         }, // Pawn
    {   9,   63,   0                    }, // Knight      OUR PIECES
    {  59,   65,  42,     0             }, // Bishop
    {  46,   39,  24,   -24,    0       }, // Rook
    {  97,  100, -42,   137,  268,    0 }  // Queen
  };

----

int QO1=1438, QO2=40, QO3=38, QO4=32, QO5=255, QO6=-62, QO7=0, QO8=104, QO9=4, QO10=0, QO11=-26;
int QO12=-2, QO13=47, QO14=105, QO15=-208, QO16=-189, QO17=24, QO18=117, QO19=133, QO20=-134, QO21=-6;

int QT1=36, QT2=9, QT3=63, QT4=59, QT5=65, QT6=42, QT7=46, QT8=39, QT9=24, QT10=-24;
int QT11=97, QT12=100, QT13=-42, QT14=137, QT15=268;
  constexpr int QuadraticOurs[][PIECE_TYPE_NB] = {
    //            OUR PIECES
    // pair pawn knight bishop rook queen
    { QO1                               }, // Bishop pair
    { QO2,  QO3                         }, // Pawn
    { QO4,  QO5, QO6                    }, // Knight      OUR PIECES
    { QO7,  QO8, QO9,  QO10             }, // Bishop
    {QO11, QO12,QO13,  QO14,  QO15      }, // Rook
    {QO16, QO17,QO18,  QO19,  QO20,QO21 }  // Queen
  };

  constexpr int QuadraticTheirs[][PIECE_TYPE_NB] = {
    //           THEIR PIECES
    // pair pawn knight bishop rook queen
    {   0                               }, // Bishop pair
    { QT1,    0                         }, // Pawn
    { QT2,  QT3,   0                    }, // Knight      OUR PIECES
    { QT4,  QT5, QT6,     0             }, // Bishop
    { QT7,  QT8, QT9,  QT10,    0       }, // Rook
    {QT11, QT12,QT13,  QT14, QT15,    0 }  // Queen
  };

TUNE(SetDefaultRange, QO1, QO2, QO3, QO4, QO5, QO6, QO7, QO8, QO9, QO10, QO11);
TUNE(QO12, QO13, QO14, QO15, QO16, QO17, QO18, QO19, QO20, QO21);

TUNE(QT1, QT2, QT3, QT4, QT5, QT6, QT7, QT8, QT9, QT10);
TUNE(QT11, QT12, QT13, QT14, QT15);





