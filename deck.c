#include "deck.h"
#include <stdlib.h>
#include <time.h>

void deck_init(Deck *deck) {
    deck->top = 0;
    for (int i = 0; i < DECK_SIZE; i++) {
        deck->cards[i].value = i + 1;
        deck->cards[i].heads = card_calculate_heads(i + 1);
    }
}

void deck_shuffle(Deck *deck) {
    srand(time(NULL));
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
    deck->top = 0;
}

Card deck_draw(Deck *deck) {
    if (deck->top < DECK_SIZE) {
        return deck->cards[deck->top++];
    }
    // Retourner une carte vide ou gérer l'erreur si le paquet est vide
    Card empty = {0, 0};
    return empty;
}