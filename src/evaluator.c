#include "../lib/eval_arrays.h"
#include "../lib/evaluator.h"

 //Print type of each card in hand to standard output.
void print_hand(int* hand, int n)
{

  int i, r;
  char suit;
  static char *rank = "23456789TJQKA";

  for (i = 0; i < n; i++, hand++) {
    r = (*hand >> 8) & 0xF;
    if (*hand & 0x8000)
      suit = 'c';
    else if (*hand & 0x4000)
      suit = 'd';
    else if (*hand & 0x2000)
      suit = 'h';
    else
      suit = 's';

    printf("%c%c ", rank[r], suit);
  }
  printf("\n");
}

//Return hand rank enum value to use in value_str string array.
int hand_rank(unsigned short val)
{

    if (val > 6185)
	return HIGH_CARD; // 1277 high card
    if (val > 3325)
	return ONE_PAIR; // 2860 one pair
    if (val > 2467)
	return TWO_PAIR; //  858 two pair
    if (val > 1609)
	return THREE_OF_A_KIND; //  858 three-kind
    if (val > 1599)
	return STRAIGHT; //   10 straights
    if (val > 322)
	return FLUSH; // 1277 flushes
    if (val > 166)
	return FULL_HOUSE; //  156 full house
    if (val > 10)
	return FOUR_OF_A_KIND; //  156 four-kind
    return STRAIGHT_FLUSH;   //   10 straight-flushes
}

// Paul Senzee's perfect hash lookup.
unsigned find_fast(unsigned u)
{
  
    unsigned a, b, r;

    u += 0xe91aaa35;
    u ^= u >> 16;
    u += u << 8;
    u ^= u >> 4;
    b = (u >> 8) & 0x1ff;
    a = (u + (u << 2)) >> 19;
    r = a ^ hash_adjust[b];
    
    return r;
}

// Cactus Kev's magic evaluation
int eval_5hand_fast(int c1, int c2, int c3, int c4, int c5)
{
    int q = (c1 | c2 | c3 | c4 | c5) >> 16;
    short s;

    // check for flushes and straight flushes
    if (c1 & c2 & c3 & c4 & c5 & 0xf000)
	return flushes[q];

    // check for straights and high card hands
    if ((s = unique5[q]))
	return s;

    // otherwise use hash-based lookup
    return hash_values[find_fast((c1 & 0xff) * (c2 & 0xff) * (c3 & 0xff) *
                               (c4 & 0xff) * (c5 & 0xff))];
}

// Unoptimized 7-card hand evaluation.
unsigned short eval_7hand(int* hand)
{
  
    int i, j, subhand[5];
    unsigned short q, best = 9999;

    for (i = 0; i < 21; i++) {
	for (j = 0; j < 5; j++) {
	    subhand[j] = hand[perm7[i][j]];
	}
	q = eval_5hand_fast(subhand[0], subhand[1], subhand[2], subhand[3],
                        subhand[4]);
	if (q < best)
	    best = q;
    }
    return best;
}
