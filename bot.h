#ifndef BOT_H
#define BOT_H


#define MAX_HAND_SIZE 10

typedef struct {
    Card hand[MAX_HAND_SIZE];
    int nb_card;
    Line current_line[4];
} BotHand;


int choix_defaut();
int choix_intelligent();
int choix_ligne(Line *lines, int num_lines);
int init_bot(int id);

#endif