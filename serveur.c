#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "deck.h"
#include "line.h"
#include "player.h"
#include "game.h"
#include "bot.h"

#define MAX_MSG 256

void create_fifos(int n)
{
    for(int i = 0;i<n;i++)
    {

    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <nb_joueur> <nb_client> <nb_manche> <option_team>\n", argv[0]);
        exit(1);
    }
    
    int n = atoi(argv[1]);
    int opt_team = atoi(argv[4]);

    if(opt_team==1 && n%2!=0){
        printf("Le nombre de joueur doit être paire pour un partie en équipe");

    }

    int nb_loop = atoi(argv[3]);
    int in_fd[n], out_fd[n];
    int nb_client = atoi(argv[2]);

    char fifo_in[64], fifo_out[64];

    // Ouvrir les FIFOs de tous les clients
    for (int i = 0; i < n; i++) {
        sprintf(fifo_in,  "client_%d_out", i);
        sprintf(fifo_out, "client_%d_in",  i);

        unlink(fifo_in);
        unlink(fifo_out);

        if(mkfifo(fifo_in,0666)==-1 && errno != EEXIST){perror(fifo_in);exit(1);}
        if(mkfifo(fifo_out,0666)==-1 && errno != EEXIST){perror(fifo_out);exit(1);}

        in_fd[i]  = open(fifo_in, O_RDWR);
        out_fd[i] = open(fifo_out, O_RDWR);

        if (in_fd[i] < 0 || out_fd[i] < 0) {
            perror("open");
            exit(1);
        }

    }

    char buffer[MAX_MSG];

    printf("Serveur prêt.\n");


    int connecter = 0;
    while (connecter != nb_client) {
        for (int i = 0; i < nb_client; i++) {
            int r = read(in_fd[i], buffer, sizeof(buffer));
            if (r > 0) {
                if(strncmp(buffer,"HELLO", 5)==0)
                {
                    printf("Serveur : client %d connecté.\n", i);
                    connecter++;
                }
                //else{
                //    buffer[r] = '\0';
                //    printf("[Serveur] reçu du client %d : %s", i, buffer);
                //    for (int j = 0; j < n; j++) {
                //        if (j != i) write(out_fd[j], buffer, strlen(buffer));
                //    } 
                //}
            }
        }

        sleep(1);
    }
    for(int i = nb_client;i<n;i++)
    {
        int pid;
        pid = fork();
        if(pid<0){perror("fork");exit(1);}
        else if(pid ==0){init_bot(i);exit(0);}
        else
        {
            int r = read(in_fd[i], buffer, sizeof(buffer));
            if (r > 0) {
            if(strncmp(buffer,"HELLO", 5)==0)
                {
                    printf("Serveur : bot %d connecté.\n", i);
                    connecter++;
                }
            }
        }
        
    }

    Deck deck;
    Player lst_p[n];
    Line tab[4];
    game_init(&deck,lst_p,n,tab,nb_loop,10,in_fd,out_fd,opt_team);

    return 0;
}
