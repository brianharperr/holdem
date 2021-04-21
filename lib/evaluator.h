#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stdio.h>

//Hand definitions
#define STRAIGHT_FLUSH 1
#define FOUR_OF_A_KIND 2
#define FULL_HOUSE 3
#define FLUSH 4
#define STRAIGHT 5
#define THREE_OF_A_KIND 6
#define TWO_PAIR 7
#define ONE_PAIR 8
#define HIGH_CARD 9

//Hand strings
const static char *value_str[] = {
    "", "Straight Flush",  "Four of a Kind", "Full House", "Flush",
    "Straight", "Three of a Kind", "Two Pair",       "One Pair",   "High Card"};

unsigned short eval_7hand(int* hand); // Unoptimized 7-card hand evaluation.
int eval_5hand_fast(int c1, int c2, int c3, int c4, int c5); // Cactus Kev's magic evaluation
unsigned find_fast(unsigned u); // Paul Senzee's perfect hash lookup.
int hand_rank(unsigned short val); // Returns hand rank enum value to use in value_str string array.
void print_hand(int* hand, int n); // Print type of each card in hand to stdout.

#endif
