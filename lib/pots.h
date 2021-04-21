#ifndef POTS_H
#define POTS_H

#include <stdlib.h>

typedef struct Pot
{
    int max;
    int creator;
    int* members;
} Pot;

typedef struct PotList
{
    int size;
    Pot* list;
} PotList;

PotList* pot_list_create( int num_players ); // Creates the pot list, allocates memory for one Pot and fills it with its members.
void pot_list_add( PotList* p, int num_members, int max, int creator ); // Define the maximum amount for current Pot and the "all-in" player and adds a Pot to the PotList.
void pot_list_sort( PotList* p, int num_players ); // Adds correct members to each pot and uses an insertion sort (ascending-order) to make distribution of winnings easier.
void pot_list_free( PotList* p ); // Frees memory allocated to the PotList.

#endif
