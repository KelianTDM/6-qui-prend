#include "line.h"
#include "deck.h"
#include <stdio.h>

void line_init(Line *l, Deck *d) {
    l->n = 1;
    l->cards[0] = deck_draw(d);
}

void line_add_card(Line *l, Card c) {
    if (l->n < MAX_LINE_LEN) {
        l->cards[l->n] = c;
        l->n++;
    }
}

int line_is_full(Line *l) {
    return l->n == MAX_LINE_LEN;
}

int line_last_value(Line *l) {
    if (l->n == 0) return -1;
    return l->cards[l->n - 1].value;
}

void line_clear(Line *l) {
    l->n = 0;
}

void line_print(Line *l, int id) {
    printf("Ligne %d: ", id);
    for (int i = 0; i < l->n; i++) {
        printf("%d(%d) ", l->cards[i].value,l->cards[i].heads);
    }
    printf("\n");
}
