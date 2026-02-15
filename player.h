#ifndef PLAYER_H
#define PLAYER_H

#include "deck.h"

#define HAND_SIZE 10

typedef struct {
    int id;
    Card hand[HAND_SIZE];
    int score;
    int nb_card;
    int  id_mate; // -1 pas d'équipe
} Player;

void player_init(Player *player, int id, Deck *deck, int score, int id_mate);
void player_add_mate(Player *players,int nb_players);
void player_remove_card(Player *player, int card_index);
int players_finish(Player *players,int nb_player);

#endif
