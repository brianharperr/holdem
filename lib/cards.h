//cards.h 

#ifndef CARDS_H
#define CARDS_H

#include <stdio.h>    // Console I/O
#include <stdlib.h>   // Dynamic memory allocation
#include <stdbool.h>  // Boolean data type
#include <string.h>   // String data type
#include <time.h>     // Current time data used by mersenne twister
#include "mtwister.h" // Mersenne Twister Random Number Generation

//Rank definitions
#define Deuce 0
#define Trey 1
#define Four 2
#define Five 3
#define Six 4
#define Seven 5
#define Eight 6
#define Nine 7
#define Ten 8
#define Jack 9
#define Queen 10
#define King 11
#define Ace 12

//Get rank with mask
#define RANK(x) ((x >> 8) & 0xF)

//Suite definitions
#define CLUB 0x8000
#define DIAMOND 0x4000
#define HEART 0x2000
#define SPADE 0x1000

typedef int Card;
typedef Card* Cards;

//struct: Deck
//Holds pointer used to address an array of Cards(ints) and a char for the top card.
typedef struct Deck
{
    Cards cards;
    char top_idx;
} Deck;

Deck* deck_create();     // Returns a pointer to a standard 52-card deck.
Card card_deal(Deck* d); // Returns a Card from the top of the Deck, goes to next card, reshuffles Deck if at last card.
void shuffle(Deck* d);   // Shuffles the Deck using Mersenne RNG and a basic swap.
void deck_free(Deck* d); // Frees memory allocated to a Deck.

#endif
