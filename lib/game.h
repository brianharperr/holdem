#ifndef GAME_H
#define GAME_H

#include "cards.h"
#include "pots.h"
#include "evaluator.h"

#define MIN_NUM_PLAYERS 3
#define DEFAULT_NUM_PLAYERS 5
#define MAX_NUM_PLAYERS 9

#define DEFAULT_MIN_BET 50
#define DEFAULT_STARTING_STACK 5000

enum{ PREFLOP, FLOP, TURN, RIVER, END };
enum{ CALL, RAISE, FOLD };
typedef struct Data
{
    int num_players;
    int start_stack;
    int min_bet;
    int curr_min_bet;
    int total_pot;
    int round;
    Card community[5];
} Data;

typedef struct Idxs
{
    char dealer_idx;
    char turn_idx;
    char end_player_idx;
} Idxs;

typedef struct Player
{
    int stack;
    int contributed;
    bool all_in;
    bool out;
    Card hand[2];
} Player;

typedef Player* Table;

//Holds all the data used in this game.
typedef struct Game
{
    Deck* deck;
    PotList* pot_list; 
    Table table;
    Data gd;
    Idxs gi;
} Game;

void TexasHoldEm(); // Console game.

Game* game_create( int num_players, int start_stack, int min_bet ); // Create a Game struct for No-Limits Texas Hold 'Em.
void game_free( Game* g ); // Frees memory allocated to a Game.

void round_handler( Game* g ); // Manage the logic for each round: PREFLOP, FLOP, TURN, RIVER, END.

void new_hand( Game* g ); // Resets data to play another hand.

void bet( Game* g, int bet ); // Simple bet.

void call( Game* g ); // Call the current minimum bet.
void raise_bet( Game* g, int amount ); // Raise the minimum bet to a certain amount.
void fold( Game* g ); // Kill cards.

void small_blind( Game* g ); // Forced bet at min_bet / 2.
void big_blind( Game* g ); // Forced bet at min_bet.

void deal_hole_cards( Game* g ); // Deal 2 hole cards to each player.
void deal_community_cards( Game* g, int round ); // Deal FLOP, TURN, and RIVER Cards.

void ai_decision( Game* g ); // Extremely basic RNG-based AI.

void winnings_distribute( Game* g ); // Distribute the winnings to the winner of each pot.
unsigned short* pot_winner( Game* g, Pot p ); // Get winner of an individual pot.

int check_table_fold( Game* g );
void force_end( Game* g ); // Forces rounds to END, used when (# players - 1) has gone all in.
int playing_count( Game* g ); // Counts how many players are still able to bet. (Not folded and not all in).
bool need_new_pot( Game* g, int amount ); // Check if amount bet is greater than any side pot maxes.
void contributions_clear( Game* g ); // Clear contributions, used at end of each betting round.
void next_turn( Game* g ); // Set the current turn to the next player.
void end_player_set( Game* g ); // Set which player needs to call/fold to end the betting round.
void next_dealer( Game* g ); // Set dealer to next player, used at the beginning of a new hand.
#endif
