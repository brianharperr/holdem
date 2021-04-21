#include "../lib/cards.h"

// Returns a pointer to a standard 52-card deck.
Deck* deck_create()
{
    

    //Allocate memory
    Deck* d = malloc( sizeof(*d) );
    if(d == NULL)
	return NULL;

    d->cards = malloc( sizeof( *(d->cards) ) * 52 );
    if(d->cards == NULL)
    {
	free(d);
	return NULL;
    }
    
    //Set top-index to very first Card.
    d->top_idx = 0;

    //Fill Deck with Cards(ints) aligned with Cactus Kev's format.
    int idx = 0;
    int suit = 0x8000;
    int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};

    for (int i = 0; i < 4; ++i, suit >>= 1) {
	for (int j = 0; j < 13; ++j, ++idx) {
	    d->cards[idx] = primes[j] | (j << 8) | suit | (1 << (16 + j));
	}
    }

    return d;
}

// Returns a Card from the top of the Deck, goes to next card, reshuffles Deck if at last card.
Card card_deal(Deck* d)
{
    //Get top card
    int tmp = d->top_idx;

    //If card is last in deck, shuffle; if not set next card as top.
    if(tmp == 51)
    {
	shuffle(d);
    }else{
	d->top_idx++;
    }

    return d->cards[tmp];
}

// Shuffles the Deck using Mersenne RNG and a basic swap.
void shuffle(Deck* d) {

    time_t t;
    MTRand rng = seedRand((unsigned)time(&t));

    int idx = 0;

    while (idx < 52) {

	int rand_idx = (int)(genRand(&rng) * 52);

	int tmp = d->cards[idx];
	d->cards[idx] = d->cards[rand_idx];
	d->cards[rand_idx] = tmp;

	idx++;
    }

    d->top_idx = 0;

}

//Frees memory allocated to a Deck.
void deck_free(Deck* d)
{
    free(d->cards);
    free(d);
}
