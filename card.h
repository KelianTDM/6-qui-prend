#ifndef CARD_H
#define CARD_H

typedef struct {
    int value; // 1 à 104
    int heads; // Têtes de boeuf (1, 2, 3, 5 ou 7)
} Card;

int card_calculate_heads(int value);

#endif