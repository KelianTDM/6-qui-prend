#ifndef LINE_H
#define LINE_H
#include "card.h"
#include "deck.h"
#define MAX_LINE_LEN 6  

typedef struct {
    Card cards[MAX_LINE_LEN];
    int n;
} Line;

void line_init(Line *l, Deck *d);

void line_add_card(Line *l, Card c);

int line_is_full(Line *l);

int line_last_value(Line *l);

int line_total_bulls(Line *l);

void line_clear(Line *l);

void line_print(Line *l, int id);

#endif
