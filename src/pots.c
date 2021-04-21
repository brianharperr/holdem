#include "../lib/pots.h"

// Creates the pot list, allocates memory for one Pot and fills it with its members.
PotList* pot_list_create( int num_players )
{

    //Allocate memory for first pot.
    PotList* p = malloc( sizeof(*p) );
    p->list = malloc( sizeof(Pot) );
    p->list[0].members = malloc( sizeof( *(p->list[0].members) ) * num_players );
    p->list[0].max = 0;
    //Fill first pot with every player.
    for( int i = 0; i < num_players; ++i )
    {
	p->list[0].members[i] = i;
    }

    p->size = 1;

    return p;
}

// Define the maximum amount for current Pot and the "all-in" player and adds a Pot to the PotList.
void pot_list_add( PotList* p, int num_members, int max, int creator )
{
    
    p->list[p->size - 1].max = max;
    p->list[p->size - 1].creator = creator;
    p->size++;

    p->list = realloc( p->list, sizeof( Pot ) * p->size );
    p->list[p->size - 1].members = malloc( sizeof( *(p->list[p->size - 1].members) ) * num_members );
    p->list[p->size - 1].max = 0;
    for( int i = 0; i < num_members; ++i)
    {
	   p->list[p->size - 1].members[i] = i;
    }

}

// Adds correct members to each pot and uses an insertion sort (ascending-order) to make distribution of winnings easier.
void pot_list_sort( PotList* p, int num_players )
{
    
    Pot tmp;
    int i, j, creator_buff;
    int flag = 0;

    //Insertion sort (ascending-order) based on pots' maxes.
    for( i = 1; i <= p->size - 2; ++i)
    {
	tmp = p->list[i];

	for( j = i - 1; j >= 0; --j)
	{
	    if( p->list[j].max > tmp.max )
	    {
		p->list[j+1] = p->list[j];
		flag = 1;
	    }else{
		break;
	    }
	}

	if(flag)
	    p->list[j+1] = tmp;
    }

    //Fill members.
    for( i = 0; i < p->size; ++i)
    {
	creator_buff = p->list[i].creator;

	for( j = i + 1; j < p->size; ++j)
	{
	    
	    for( int x = 0; x < num_players; ++x)
	    {

		if( p->list[j].members[x] == creator_buff )
		    p->list[j].members[x] = -1;

	    }

	}

    }



}

// Frees memory allocated to the PotList.
void pot_list_free( PotList* p )
{
    for( int i = 0; i < p->size; ++i )
    {
	   free( p->list[i].members );
    }
    free( p->list );
    free( p );

}
