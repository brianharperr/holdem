#include "../lib/game.h"

void TexasHoldEm()
{

	Game* g = game_create( 0, 0, 0 );
	int options;
	
	while(1)
	{
		if(g->gd.round == PREFLOP){
			round_handler( g );
		}
		printf("Player %d: %d : ", g->gi.turn_idx, g->table[g->gi.turn_idx].stack);
		scanf(" %d", &options);
		if( options > 0 )
		{
			raise_bet( g, options);
		}
		else if(options == 0){
			if(g->gi.turn_idx == g->gi.end_player_idx){
				call( g );
				round_handler( g );
			}else{
				call( g );
			}
		}
		else{
			if(g->gi.turn_idx == g->gi.end_player_idx){
				fold( g );
				round_handler( g );
			}else{
				fold( g );
			}
		}
	}
	game_free( g );
}

// Creates a Game struct for No-Limits Texas Hold 'Em.
Game* game_create( int num_players, int start_stack, int min_bet )
{
    Game* g = malloc( sizeof(*g) );
    if(g == NULL)
	return NULL;

    g->deck = deck_create();
    if(g->deck == NULL)
    {
	free(g);
	return NULL;
    }

    if( num_players == 0 && start_stack == 0 && min_bet == 0 )
    {
	g->gd.num_players = DEFAULT_NUM_PLAYERS;
	g->gd.start_stack = DEFAULT_STARTING_STACK;
	g->gd.min_bet = DEFAULT_MIN_BET;
    
    }else if( num_players <= MAX_NUM_PLAYERS && num_players >= MIN_NUM_PLAYERS )
    {

	g->gd.num_players = num_players;
	g->gd.start_stack = start_stack;
	g->gd.min_bet = min_bet;

    }else
    {

	printf("Error: Number of players must be in range of [3-9]");
	exit(0);

    }

    g->gd.round = PREFLOP;
    g->gd.curr_min_bet = g->gd.min_bet;
    g->gd.total_pot = 0;

    g->pot_list = pot_list_create( g->gd.num_players );
    if(g->pot_list == NULL)
    {
	deck_free( g->deck );
	free( g );
	return NULL;
    }

    g->table = malloc( sizeof( *(g->table) ) * g->gd.num_players );
    if(g->table == NULL)
    {
	pot_list_free( g->pot_list );
	deck_free( g->deck );
	free( g );
	return NULL;
    }

    for( int i = 0; i < g->gd.num_players; ++i)
    {
	g->table[i].stack = g->gd.start_stack;
	g->table[i].out = 0;
	g->table[i].all_in = 0;
    }

    //Pick a random player to be the first dealer.
    time_t t;
    MTRand rng = seedRand( (unsigned)time( &t ) );
    g->gi.dealer_idx = ( int )( genRand( &rng ) * g->gd.num_players );
    g->gi.turn_idx = g->gi.dealer_idx;

    //Set the first turn to the player with the small blind obligation (left of dealer).
    next_turn( g );
    
    return g;
}

// Frees memory allocated to a Game.
void game_free( Game* g )
{
    free( g->table );
    pot_list_free( g->pot_list );
    deck_free( g->deck );
    free( g );
}

//Play a new hand. 
void new_hand( Game* g  )
{
	int sum_playing = 0;
    for( int i = 0; i < g->gd.num_players; ++i )
    {

		g->table[i].all_in = 0;
		if( g->table[i].stack > 0 )
		{
		    g->table[i].out = 0;
		    sum_playing++;
		}else{
			g->table[i].out = 1;
		}
    }

    if(sum_playing == 1)
    {
    	printf("There are no opponents left to clean.\n");
    }
    contributions_clear( g );
    next_dealer( g );
    g->gi.turn_idx = g->gi.dealer_idx;
    g->gd.curr_min_bet = g->gd.min_bet;
    next_turn( g );
    //FIX THIS MEMORY 
    pot_list_free( g->pot_list );
    g->pot_list = pot_list_create( g->gd.num_players );  
}

// Execute the current round.
void round_handler( Game* g )
{

    switch( g->gd.round )
    {
	case PREFLOP:

	    new_hand( g ); 
	    shuffle( g->deck );
	    contributions_clear( g );

	    small_blind( g );
	    big_blind( g ); 

	    deal_hole_cards( g );

	    printf("HOLE CARDS: "); print_hand( g->table[0].hand, 2);

	    break;
	case FLOP:

		g->gd.curr_min_bet = g->gd.min_bet;
		contributions_clear( g );

	    deal_community_cards( g, FLOP );

	    print_hand( g->gd.community, 3);

	    break;
	case TURN:

		g->gd.curr_min_bet = g->gd.min_bet;
		contributions_clear( g );

	    deal_community_cards( g, TURN );

	    print_hand( g->gd.community, 4);

	    break;
	case RIVER:

		g->gd.curr_min_bet = g->gd.min_bet;
		contributions_clear( g );

	    deal_community_cards( g, RIVER );

	    print_hand( g->gd.community, 5);

	    break;
	case END:

		winnings_distribute( g );
	    break;
    }

    if(g->gd.round == END){
		g->gd.round = PREFLOP;
		round_handler( g );
    }else{
		g->gd.round++;
    }
}

// Simple bet.
void bet( Game* g, int bet )
{
    g->table[g->gi.turn_idx].stack -= bet;
    g->table[g->gi.turn_idx].contributed += bet;
    g->gd.total_pot += bet;
}

// Forced bet at min_bet / 2.
void small_blind( Game* g )
{
    bet( g, g->gd.min_bet / 2 );
    next_turn( g );
}

// Forced bet at min_bet.
void big_blind( Game* g )
{
    bet( g, g->gd.min_bet );
    end_player_set( g );
    next_turn( g );
}

// Call the current minimum bet.
void call( Game* g )
{
	if( (g->gd.curr_min_bet - g->table[g->gi.turn_idx].contributed) >= g->table[g->gi.turn_idx].stack )
	{
		if(need_new_pot( g, g->table[g->gi.turn_idx].stack + g->table[g->gi.turn_idx].contributed ))
		{
			printf("Call Pot added.\n");
			pot_list_add( g->pot_list, g->gd.num_players, g->table[g->gi.turn_idx].stack + g->table[g->gi.turn_idx].contributed, g->gi.turn_idx );
		}
		g->table[g->gi.turn_idx].all_in = 1;
		bet(g, g->table[g->gi.turn_idx].stack);
	}else
	{
      bet(g, g->gd.curr_min_bet - g->table[g->gi.turn_idx].contributed);
    }

    if(playing_count( g ) == 0)
    {
    	force_end(g);
    }else{
    	next_turn(g);
    }
}

// Raise the minimum bet to a certain amount.
void raise_bet( Game* g, int amount )
{

	if( amount > g->table[g->gi.turn_idx].stack )
	{
		printf("Invalid raise: Raised more chips are in stack.\n");
		return;
	}

	if( amount < g->gd.curr_min_bet )
	{
		printf("Invalid raise: Must raise more than the minimum bet.\n");
		return;
	}
	else if( amount == g->gd.curr_min_bet)
	{
		call( g );
		return;
	}
	else
	{

		if(amount == g->table[g->gi.turn_idx].stack)
		{
			if(need_new_pot( g, g->table[g->gi.turn_idx].stack + g->table[g->gi.turn_idx].contributed))
			{
				printf("Raise Pot added.\n");
				pot_list_add( g->pot_list, g->gd.num_players, g->table[g->gi.turn_idx].stack, g->gi.turn_idx );
			}
   			g->table[g->gi.turn_idx].all_in = 1;

		}

		g->gd.curr_min_bet = amount;
	 	bet(g, g->gd.curr_min_bet);
  		if(playing_count( g ) == 0)
    	{
	    	force_end(g);
	    }else{
	    	end_player_set(g);
	    	next_turn(g);
	    }

	}
}

// Kill cards.
void fold( Game* g )
{
	g->table[g->gi.turn_idx].out = 1;
	printf("Playing: %d\n", playing_count( g ));
    if(playing_count( g ) == 0)
    {
    	force_end(g);
    }else{
    	int winner = check_table_fold( g );
    	if( winner != -1 )
    	{
    		printf("PLAYER %d WINS %d chips.\n", winner, g->gd.total_pot);
    		g->table[winner].stack += g->gd.total_pot;
    		g->gd.total_pot = 0;

    		g->gd.round = PREFLOP;
    	}
    	if (g->gi.turn_idx == g->gi.end_player_idx) {
          end_player_set(g);
    	}
		next_turn(g);
    	
    }
}

// Deal 2 hole cards to each player.
void deal_hole_cards( Game* g )
{

    for( int i = 0; i < g->gd.num_players; ++i)
    {
	if( !(g->table[i].out) )
	{
	    g->table[i].hand[0] = card_deal( g->deck );
	    g->table[i].hand[1] = card_deal( g->deck );
	}
    }
}

// Deal FLOP, TURN, and RIVER Cards.
void deal_community_cards( Game* g, int round)
{

    switch( round )
    {
	case FLOP:
	    g->gd.community[0] = card_deal( g->deck );
	    g->gd.community[1] = card_deal( g->deck );
	    g->gd.community[2] = card_deal( g->deck );
	    break;
	case TURN:
	    g->gd.community[3] = card_deal( g->deck );
	    break;
	case RIVER:
	    g->gd.community[4] = card_deal( g->deck );
	    break;
    }
}

// Distribute the winnings to the winner of each pot.
void winnings_distribute( Game* g )
{
	pot_list_sort( g->pot_list, g->gd.num_players );
	unsigned short* winner = malloc(sizeof(*winner) * 2);
	if(g->pot_list->size == 1){
		winner = pot_winner( g, g->pot_list->list[0] );
		printf("typ1:WINNER OF %d-POT: PLAYER %d WINS %d chips.\n", g->pot_list->list[0].max, winner[0], g->gd.total_pot);
		g->table[winner[0]].stack += g->gd.total_pot;
		g->gd.total_pot = 0;
	}else{
		for( int i = 0; i < g->pot_list->size; ++i )
		{
			winner = pot_winner( g, g->pot_list->list[i] );
			if(g->pot_list->list[i].max == 0)
			{
				printf("typ2:WINNER OF %d-POT: PLAYER %d WINS %d chips.\n", g->pot_list->list[i].max, winner[0], g->gd.total_pot);
				g->table[winner[0]].stack += g->gd.total_pot;
				g->gd.total_pot = 0;

			}else{
				int tmp = g->table[winner[0]].stack;
				if(g->gd.total_pot < g->pot_list->list[i].max * g->gd.num_players){
					g->table[winner[0]].stack += g->gd.total_pot;
					g->gd.total_pot = 0;
				}else{
					g->table[winner[0]].stack += g->pot_list->list[i].max * g->gd.num_players;
					g->gd.total_pot -= g->pot_list->list[i].max * g->gd.num_players;
				}
				printf("typ3:WINNER OF %d-POT: PLAYER %d WINS %d chips.\n", g->pot_list->list[i].max, winner[0], g->table[winner[0]].stack - tmp);
			}
		}
	}
	if(winner != NULL)
	{
		free(winner);
	}
}

// Get hand and index of winner of an individual pot.
unsigned short* pot_winner( Game* g, Pot p )
{
  unsigned short *winner_stats = malloc((sizeof *winner_stats) * 2);
  unsigned short q;
  unsigned short best = 9999;
  unsigned short best_player_idx = 0;
  int hand[7] = {g->gd.community[0],
                 g->gd.community[1],
                 g->gd.community[2],
                 g->gd.community[3],
                 g->gd.community[4],
                 0,
                 0};


  for(int j = 0; j < g->gd.num_players; ++j)
  {
      if(p.members[j] != -1 && !(g->table[j].out))
      {

        hand[5] = g->table[j].hand[0];
        hand[6] = g->table[j].hand[1];
        q = eval_7hand(hand);
        if (q < best) {
          best = q;
          best_player_idx = j;
        }

      }
    }

  winner_stats[0] = best_player_idx;
  winner_stats[1] = best;

  return winner_stats;
}

// Check if all other players folded
int check_table_fold( Game* g )
{

	int sum = 0;
	int idx = -1;
	for(int i = 0; i < g->gd.num_players; ++i)
	{
		if(g->table[i].out)
		{
			sum++;
		}else{
			idx = i;
		}
	}

	if( sum == g->gd.num_players - 1)
	{
		return idx;
	}else{
		return -1;
	} 
}

// Check if amount bet is greater than any side pot maxes.
bool need_new_pot( Game* g, int amount )
{
	bool tmp = true;

	for(int i = 0; i < g->pot_list->size; ++i)
	{
		if( amount == g->pot_list->list[i].max)
		{
			tmp = false;
			break;
		}
	}
	return tmp;
}

// Forces rounds to END, used when (# players - 1) has gone all in.
void force_end( Game* g )
{
	while(g->gd.round != END){
		round_handler( g );
	}
}

// Counts how many players are still able to bet. (Not folded and not all in).
int playing_count( Game* g )
{
	int sum = 0;
	for(int i = 0; i < g->gd.num_players; ++i)
	{
		if(!(g->table[i].out || g->table[i].all_in))
		{
			sum++;
		}
	}

	return sum;
}

// Clear contributions, used at end of each betting round.
void contributions_clear( Game* g )
{
	for( int i = 0; i < g->gd.num_players; ++i)
	{
		g->table[i].contributed = 0;
	}
}

// Set next player to be dealer.
void next_dealer( Game* g )
{
    do
    {

	if( g->gi.dealer_idx < g->gd.num_players - 1 )
	{

	    g->gi.dealer_idx++;

	}else
	{

	    g->gi.dealer_idx = 0;

	}

    }while( g->table[g->gi.dealer_idx].out );
}

// Sets the current turn to the next player.
void next_turn( Game* g )
{
    
    do
    {

	if( g->gi.turn_idx < g->gd.num_players - 1 )
	{

	    g->gi.turn_idx++;

	}else
	{

	    g->gi.turn_idx = 0;

	}

    }while( g->table[g->gi.turn_idx].out || g->table[g->gi.turn_idx].all_in );
}

// Set which player needs to call/fold to end the betting round.
void end_player_set( Game* g )
{

    int tmp = g->gi.turn_idx;

    do
    {

	if(tmp > 0)
	{
	    tmp--;
	}else
	{
	    tmp = g->gd.num_players - 1;    
	}

    }while( g->table[tmp].out || g->table[tmp].all_in );
    
    g->gi.end_player_idx = tmp;
}