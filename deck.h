#ifndef DECK_H
#define DECK_H

#include "card.h"

#define DECK_SIZE 104

typedef struct {
    Card cards[DECK_SIZE];
    int top;
} Deck;

void deck_init(Deck *deck);
void deck_shuffle(Deck *deck);
Card deck_draw(Deck *deck);

#endif