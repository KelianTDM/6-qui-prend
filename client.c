#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include "message.h"
#include "card.h"
#include "line.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <client_id>\n", argv[0]);
        exit(1);
    }

    int id = atoi(argv[1]);
    char fifo_in[64], fifo_out[64];

    sprintf(fifo_in,  "client_%d_in",  id);
    sprintf(fifo_out, "client_%d_out", id);

    int fd_in  = open(fifo_in,  O_RDWR);
    int fd_out = open(fifo_out, O_RDWR);

    if (fd_in < 0 || fd_out < 0) {
        perror("open");
        exit(1);
    }

    printf("Client %d prêt.\n", id);

    //fd_set set;
    write(fd_out, "HELLO", strlen("HELLO"));
    MsgType hist;
    while (1) {
        MsgHeader hdr;
        int r = read(fd_in,&hdr,sizeof(hdr));
        if(r<0){perror("C_read_serveur");exit(1);}
        if(hdr.type == MSG_END)
        {
            int nb_players = (int)(hdr.size/sizeof(int));
            int lst_score[nb_players];
            int r = read(fd_in,&lst_score,hdr.size);
            if(r<0){perror("C_read_end_score"); exit(1);}
            int min = 100;
            int id_win;
            for(int i =0;i<nb_players;i++)
            {
                if(lst_score[i]<min)
                {
                    min = lst_score[i];
                    id_win = i;
                }
            }
            if(id == id_win || lst_score[id] == lst_score[id_win])
            {
                printf("Vous avez gagner !\n");
            }
            else
            {
                printf("Vous avez perdu\n");
            }
            return 0;
        }
        else if(hdr.type == MSG_SCORE)
        {
            int score;
            r = read(fd_in,&score,hdr.size);
            if(r<0){perror("C_read_score");exit(1);}
            printf("Voici votre score : %d\n",score);
        }
        else if(hdr.type == MSG_CARDS)
        {
            Card cards[hdr.size];
            r = read(fd_in,cards,hdr.size);
            if(r<0){perror("C_read_card");exit(1);}
            printf("Voici vos cartes : ");
            int n = (int)hdr.size/sizeof(Card);
            for(int i =0;i<n;i++)
            {
                printf("[%d]:%d ",i+1,cards[i].value);
            }
            printf("\n");
            hist = MSG_CARDS;
        }
        else if(hdr.type == MSG_LINES)
        {
            Line lines[hdr.size];
            r = read(fd_in,lines,hdr.size);
            if(r<0){perror("C_read_lines");exit(1);}
            printf("Voici les lines : \n");
            for(int i =0;i<4;i++)
            {
                line_print(&lines[i], i+1);
            }
            printf("\n");
            hist = MSG_LINES;
        }
        else if(hdr.type == MSG_WAIT)
        {
            int choice;
            MsgHeader hdr_in = {.type = MSG_INT,.size = sizeof(int)};
            if(hist == MSG_LINES)
            {
                printf("vous devez choisir une ligne : ");
                scanf("%d",&choice);
                int w = write(fd_out,&hdr_in,sizeof(hdr_in));
                if(w<0){perror("C_write_choice"); exit(1);}
                w = write(fd_out,&choice,hdr_in.size);
                if(w<0){perror("C_write_choice"); exit(1);}

            }
            else if(hist == MSG_CARDS)
            {
                printf("vous devez choisir une carte : ");
                scanf("%d",&choice);
                int w = write(fd_out,&hdr_in,sizeof(hdr_in));
                if(w<0){perror("C_write_choice"); exit(1);}
                w = write(fd_out,&choice,hdr_in.size);
                if(w<0){perror("C_write_choice"); exit(1);}

            }
        }
        else if(hdr.type == MSG_MATE)
        {
            Card cards[hdr.size];
            r = read(fd_in,cards,hdr.size);
            if(r<0){perror("C_read_mate");exit(1);}
            printf("Voici les cartes de votre coéquipier: ");
            int n = (int)hdr.size/sizeof(Card);
            for(int i =0;i<n;i++)
            {
                printf("[%d]:%d ",i+1,cards[i].value);
            }
            printf("\n");

        }
    }

    return 0;
}
