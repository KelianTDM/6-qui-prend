#include "player.h"
#include <stdlib.h>

void player_init(Player *player, int id, Deck *deck, int score,int id_mate) 
{
    player->id = id;
    player->score = score;
    player->nb_card = HAND_SIZE;
    player->id_mate = id_mate;
    for (int i = 0; i < HAND_SIZE; i++) {
        player->hand[i] = deck_draw(deck);
    }
}

void player_add_mate(Player *players,int nb_player)
{
    int n  = (int)nb_player/2;
    for(int i = 0;i<n;i++)
    {
        players[i].id_mate = i+n;
        players[i+n].id_mate = i;
    }
}

void player_remove_card(Player *player, int card_index)
{
    player->nb_card --;
    for(int i = card_index;i<player->nb_card;i++)
    {
        player->hand[i] = player->hand[i+1];
    }

}

int players_finish(Player *players,int nb_player)
{
    int loose = 1;
    for(int i = 0;i<nb_player;i++)
    {
        if (players[i].score == 66)
        {
            loose = 0;
        }
    }
    return loose;

}

/*   void player_choose_card(Player *player, int card_index) {
        if (card_index >= 0 && card_index < HAND_SIZE) {
            player->selected_card = player->hand[card_index];
            player->hand[card_index].value = 0;
        }
    }
*/