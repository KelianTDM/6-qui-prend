#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include "message.h"
#include "card.h"
#include "line.h"
#include "player.h"
#include "bot.h"



BotHand current_hand = {0};

int choix_defaut() {
    if (current_hand.nb_card <= 0) return 0;

    int min_val = 200;
    int min_index = 0; 

    for (int i = 0; i < current_hand.nb_card; i++) {
        if (current_hand.hand[i].value < min_val) {
            min_val = current_hand.hand[i].value;
            min_index = i;
        }
    }
    return min_index + 1;
}


int choix_intelligent() {
    int best_card_value = 200;
    int best_card_index = -1;
    int target_line_found = 0;

    for (int j = 0; j < 4; j++) {
        if (current_hand.current_line[j].n < 5) { 
            target_line_found = 1;

            int last_val = current_hand.current_line[j].cards[current_hand.current_line[j].n - 1].value; // Valeur de la derniere carte de la ligne

            for (int i = 0; i < current_hand.nb_card; i++) {
                int card_val = current_hand.hand[i].value;
                if (card_val > last_val) {
                    if (card_val < best_card_value) {
                        best_card_value = card_val;
                        best_card_index = i;
                    }
                }
            }
        }
    }
    
    if (best_card_index != -1) {
        return best_card_index + 1;      
    } else {
        return choix_defaut();
    }
}

int choix_ligne(Line *lines, int num_lines) {
    int min_heads = 200;
    int best_line_index = 0;

    for (int i = 0; i < num_lines; i++) {
        int current_heads = 0;
        for (int k = 0; k < lines[i].n; k++) {
            current_heads += lines[i].cards[k].heads;
        }       
        if (current_heads < min_heads) {
            min_heads = current_heads;
            best_line_index = i;
        }
    }
    return best_line_index + 1;
}


int init_bot(int id) {

    char fifo_in[64], fifo_out[64];

    sprintf(fifo_in,  "client_%d_in",  id);
    sprintf(fifo_out, "client_%d_out", id);

    int fd_in  = open(fifo_in,  O_RDWR);
    int fd_out = open(fifo_out, O_RDWR);

    if (fd_in < 0 || fd_out < 0) {
        perror("open");
        exit(1);
    }

    //fprintf(stderr, "Bot Client %d prêt.\n", id); 

    MsgType hist = 0; 

    write(fd_out, "HELLO", strlen("HELLO"));
    
    while (1) {
        MsgHeader hdr;
        int r = read(fd_in,&hdr,sizeof(hdr));
        if(r<0){perror("C_read_serveur");exit(1);}
        
        if(hdr.type == MSG_CARDS)
        {
            r = read(fd_in, current_hand.hand, hdr.size);
            if(r<0){perror("C_read_card");exit(1);}
            
            current_hand.nb_card = hdr.size / sizeof(Card);
            
            //fprintf(stderr, "Bot %d: Reçu %d cartes.\n", id, current_hand.nb_card);
            
            hist = MSG_CARDS;
        }
        else if(hdr.type == MSG_LINES)
        {
            Line received_lines[4];
            r = read(fd_in, current_hand.current_line, hdr.size);
            if(r<0){perror("C_read_lines");exit(1);}
            
            hist = MSG_LINES;
        }
        else if(hdr.type == MSG_WAIT)
        {
            int choice = 0;
            MsgHeader hdr_in = {.type = MSG_INT, .size = sizeof(int)};
            
            if(hist == MSG_LINES)
            {
                choice = choix_ligne(current_hand.current_line, 4);
            }
            else if(hist == MSG_CARDS)
            {
                choice = choix_intelligent();
            }

            int w = write(fd_out, &hdr_in, sizeof(hdr_in));
            if(w<0){perror("C_write_choice_hdr"); exit(1);}
            w = write(fd_out, &choice, hdr_in.size);
            if(w<0){perror("C_write_choice_val"); exit(1);}
        }
        else if(hdr.type ==MSG_END)
        {
            return 0;
        }
    }

    return 0;
}