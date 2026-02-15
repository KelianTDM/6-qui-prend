#include "game.h"
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>


void game_init(Deck *deck, Player *players, int nb_players, Line *lines, int nb_loop,int nb_round, int *in_fd,int *out_fd,int opt_team) {
    deck_init(deck);
    deck_shuffle(deck);

    for(int i = 0; i < nb_players; i++) {
        player_init(&players[i], i, deck,0,-1);
    }
    if(opt_team == 1){player_add_mate(players,nb_players);}

    for(int i = 0; i < 4; i++) {
        lines[i].cards[0] = deck_draw(deck);
        lines[i].n = 1;
    }

    if(nb_loop == 0)
    {   
        int cpt = 0;
        while(players_finish(players,nb_players))
        {
            cpt ++;
            deck_init(deck);
            deck_shuffle(deck);

            for(int i = 0; i < nb_players; i++) 
            {
                player_init(&players[i], i, deck,players[i].score,players[i].id_mate);
            }

            for(int i = 0; i < 4; i++) 
            {
                lines[i].cards[0] = deck_draw(deck);
                lines[i].n = 1;
            }

            game_loop(players,nb_players,lines,nb_round,in_fd,out_fd,opt_team);
            printf("Fin de la manche %d voici les scores : \n",cpt);
            for(int i = 0; i < nb_players; i++) 
            {
                printf("Joueur %d : %d points\n",i,players[i].score);
                MsgHeader hdr_score = {.type = MSG_SCORE,.size = sizeof(int)};
                int w = write(out_fd[i],&hdr_score,sizeof(hdr_score));
                if(w<0){perror("S_write_joueur_header"); exit(1);}
                w = write(out_fd[i],&players[i].score,sizeof(int));
                if(w<0){perror("S_write_joueur_score"); exit(1);}
            }
        }
    }
    else
    {
        for(int i = 0;i<nb_loop;i++)
        {
            deck_init(deck);
            deck_shuffle(deck);

            for(int i = 0; i < nb_players; i++) 
            {
                player_init(&players[i], i, deck,players[i].score,players[i].id_mate);
            }

            for(int i = 0; i < 4; i++) 
            {
                lines[i].cards[0] = deck_draw(deck);
                lines[i].n = 1;
            }

            game_loop(players,nb_players,lines,nb_round,in_fd,out_fd,opt_team);
            printf("Fin de la manche %d voici les scores : \n",i);
            for(int i = 0; i < nb_players; i++) 
            {
                printf("Joueur %d : %d points\n",i,players[i].score);
                MsgHeader hdr_score = {.type = MSG_SCORE,.size = sizeof(int)};
                int w = write(out_fd[i],&hdr_score,sizeof(hdr_score));
                if(w<0){perror("S_write_joueur_header"); exit(1);}
                w = write(out_fd[i],&players[i].score,sizeof(int));
                if(w<0){perror("S_write_joueur_score"); exit(1);}
            }
        }
        for(int j = 0; j < nb_players; j++) {
                fprintf(stdout, "LOG|%d|%d\n", players[j].id, players[j].score); 
            }
    }
    int min = 100;
    int id;
    int lst_score[nb_players];
    for(int i =0;i<nb_players;i++)
    {
        lst_score[i] = players[i].score;
        if(players[i].score<min)
        {
            min = players[i].score;
            id = i;
        }
    }
    printf("fin de la partie c'est le joueur %d qui gagne !",id);
    printf("Voici les scores : \n");
        for(int i = 0; i < nb_players; i++) 
        {
            printf("Joueur %d : %d points\n",i,players[i].score);
            MsgHeader hdr_end = {.type = MSG_END,.size = nb_players*sizeof(int)};
            int w = write(out_fd[i],&hdr_end,sizeof(hdr_end));
            if(w<0){perror("S_write_joueur_header"); exit(1);}
            w = write(out_fd[i],&lst_score,nb_players*sizeof(int));
            if(w<0){perror("S_write_joueur_end"); exit(1);}
            sleep(1);
        }
    sleep(10);

}

int game_loop(Player *players, int nb_players, Line *lines,int nb_round, int *in_fd,int *out_fd,int opt_team) {
    for(int turn = 0; turn < nb_round; turn++) {
        printf("\n--- Tour %d ---\n", turn + 1);

        Card card_choose[nb_players];
        Player player_ord[nb_players];
        pid_t pid[nb_players];
        for(int i = 0; i < nb_players; i++) 
        {
            pid[i] = fork();
            if(pid[i]<0)
            {
                perror("fork");
                exit(1);
            }
            else if(pid[i] ==0)
            {
                //envoye des lines au joueur 
                MsgHeader hdr_lines = {.type = MSG_LINES, .size = 4*sizeof(Line)};
                int w = write(out_fd[i],&hdr_lines,sizeof(hdr_lines));
                if(w<0){perror("S_write_joueur_header"); exit(1);}
                w = write(out_fd[i],lines,4*sizeof(Line));
                if(w<0){perror("S_write_joueur_lines"); exit(1);}

                //envoye des cartes du coéquipier
                if(opt_team == 1)
                {
                    int j = players[i].id_mate;
                    MsgHeader hdr_mate = {.type = MSG_MATE, .size = players[j].nb_card*sizeof(Card)};
                    w = write(out_fd[i],&hdr_mate,sizeof(hdr_mate));
                    if(w<0){perror("S_write_coéquipier_header"); exit(1);}
                    w = write(out_fd[i],players[j].hand,players[j].nb_card*sizeof(Card));
                    if(w<0){perror("S_write _coéquipier_Card"); exit(1);}
                }

                //envoye des cartes du joueur
                MsgHeader hdr_cards = {.type = MSG_CARDS, .size = players[i].nb_card*sizeof(Card)};
                w = write(out_fd[i],&hdr_cards,sizeof(hdr_cards));
                if(w<0){perror("S_write_joueur_header"); exit(1);}
                w = write(out_fd[i],players[i].hand,players[i].nb_card*sizeof(Card));
                if(w<0){perror("S_write _joueur_Card"); exit(1);}

                int choice = 0;
                do {
                    //envoye l'informations que le serveur attend une carte
                    MsgHeader hdr_wait = {.type =  MSG_WAIT, .size = 0};
                    w = write(out_fd[i],&hdr_wait,sizeof(hdr_wait));
                    if(w<0){perror("S_write_joueur_wait"); exit(1);}

                    //attend le choix du joueur
                    MsgHeader hdr_out;
                    int r = read(in_fd[i],&hdr_out,sizeof(hdr_out));
                    if(r<0){perror("S_read_joueur_header"); exit(1);}
                    if(hdr_out.type != MSG_INT){
                        w = write(out_fd[i],&hdr_wait,sizeof(hdr_wait));
                        if(w<0){perror("S_write_joueur_wait"); exit(1);}
                    }
                    r  = read(in_fd[i],&choice,sizeof(choice));
                    if(r<0){perror("S_read_joueur_choice"); exit(1);}

                } while(choice < 1 || choice > players[i].nb_card);

                exit(choice);
            }
            
        }


        for(int i = 0;i<nb_players;i++)
        {
            int status;
            pid_t finish_pid = wait(&status);
            if(WIFEXITED(status))
            {
                int choice = WEXITSTATUS(status);
                for(int j =0;j<nb_players;j++)
                {
                    if(finish_pid == pid[j])
                    {
                        printf("pid du joueur %d : %d\n",j,finish_pid);
                        card_choose[j] = players[j].hand[choice-1];
                        player_ord[j] = players[j];
                        player_remove_card(&players[j],choice-1);
                    }
                }
            }  
        }

        //trie du tableau des cartes jouer et de l'ordre des joueurs
        for(int i = 0; i < nb_players - 1; i++) {
            for(int j = 0; j < nb_players - i - 1; j++) {
                if(card_choose[j].value > card_choose[j+1].value) {
                    Card tempC = card_choose[j];
                    Player tempP = player_ord[j];
                    card_choose[j] = card_choose[j+1];
                    card_choose[j+1] = tempC;
                    player_ord[j] = player_ord[j+1];
                    player_ord[j+1] = tempP;
                }
            }
        }



        // Placer les cartes sur les lignes
        for(int i = 0; i < nb_players; i++) {
            //int id_Player = card_choose[i][1];
            printf("Joueur %d joue la carte %d\n", player_ord[i].id, card_choose[i].value);
            
            int best_line = -1;
            int min_diff = 200; // Supérieur à 104

            //Trouver la ligne avec la plus petite différence positive
            for(int l = 0; l < 4; l++) {
                int last_val = lines[l].cards[lines[l].n - 1].value;
                if(card_choose[i].value > last_val) {
                    int diff = card_choose[i].value- last_val;
                    if(diff < min_diff) {
                        min_diff = diff;
                        best_line = l;
                    }
                }
            }

            if(best_line != -1) {
                // Cas normal  
                if(lines[best_line].n < 5) {
                    // Ajout simple
                    lines[best_line].cards[lines[best_line].n] = card_choose[i];
                    lines[best_line].n++;
                } else {
                    // 6ème carte : le joueur ramasse la ligne
                    printf("6eme carte ! Le joueur %d ramasse la ligne %d.\n", player_ord[i].id, best_line + 1);
                    int points = 0;
                    for(int k = 0; k < lines[best_line].n; k++) {
                        points += lines[best_line].cards[k].heads;
                    }
                    players[player_ord[i].id].score += points;
                    if(opt_team==1)
                    {
                        players[player_ord[i].id_mate].score += points;
                    }
                    printf("Points encaisses : %d (Total: %d)\n", points, players[player_ord[i].id].score);

                    lines[best_line].n = 1;
                    lines[best_line].cards[0] = card_choose[i];
                }
            } else {
                // Cas : Carte plus petite que toutes les lignes
                //printf("Carte trop petite ! Le joueur %d doit choisir une ligne a ramasser.\n", player_ord[i].id);
                

                //envoye des lines au joueur 
                MsgHeader hdr_lines = {.type = MSG_LINES, .size = 4*sizeof(Line)};
                int w = write(out_fd[player_ord[i].id],&hdr_lines,sizeof(hdr_lines));
                if(w<0){perror("S_write_joueur_header"); exit(1);}
                w = write(out_fd[player_ord[i].id],lines,4*sizeof(Line));
                if(w<0){perror("S_write_joueur_lines"); exit(1);}

                int choice = 0;
                do {
                    //envoye l'informations que le serveur attend une carte
                    MsgHeader hdr_wait = {.type =  MSG_WAIT, .size = 0};
                    w = write(out_fd[player_ord[i].id],&hdr_wait,sizeof(hdr_wait));
                    if(w<0){perror("S_write_joueur_wait"); exit(1);}

                    //attend le choix du joueur
                    MsgHeader hdr_out;
                    int r = read(in_fd[player_ord[i].id],&hdr_out,sizeof(hdr_out));
                    if(r<0){perror("S_read_joueur_header"); exit(1);}
                    if(hdr_out.type != MSG_INT){
                        w = write(out_fd[player_ord[i].id],&hdr_wait,sizeof(hdr_wait));
                        if(w<0){perror("S_write_joueur_wait"); exit(1);}
                    }
                    r  = read(in_fd[player_ord[i].id],&choice,sizeof(choice));
                    if(r<0){perror("S_read_joueur_choice"); exit(1);}
                } while(choice < 1 || choice > 4);
                
                int line_idx = choice - 1;
                int points = 0;
                for(int k = 0; k < lines[line_idx].n; k++) {
                    points += lines[line_idx].cards[k].heads;
                }
                players[player_ord[i].id].score += points;
                if(opt_team==1)
                    {
                        players[player_ord[i].id_mate].score += points;
                    }
                printf("Points encaisses : %d (Total: %d)\n", points, players[player_ord[i].id].score);

                lines[line_idx].n = 1;
                lines[line_idx].cards[0] = card_choose[i];
            }
        }
    }
    return 0;
}