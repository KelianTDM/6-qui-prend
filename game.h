#ifndef GAME_H
#define GAME_H

#include "deck.h"
#include "player.h"
#include "line.h"

void game_init(Deck *deck, Player *players, int nb_players, Line *lines, int nb_loop,int nb_round, int *in_fd,int *out_fd, int opt_team);
int game_loop(Player *players, int nb_players, Line *lines,int nb_round, int *in_fd,int *out_fd,int opt_team);


#endif