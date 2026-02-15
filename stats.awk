BEGIN {   
    scores_finaux_total[0]; 
    parties_jouees[0];      
    parties_gagnees[0];     
    parties_perdues[0];     
    
    current_game_id = 1; 
    entries_in_current_game = 0; 
}

# Traitement des lignes LOG (LOG|ID_Joueur|Score_Final)
/LOG/ {
    # $1=LOG, $2=ID_Joueur, $3=Score_Final
    joueur = $2;
    score = $3;
    
    scores_game[current_game_id, joueur] = score;
    joueurs_game[current_game_id, joueur] = 1;
    
    entries_in_current_game++; 
    
    if (entries_in_current_game == N) {
        
        # Détermination du Gagnant pour la partie qui vient de se terminer
        min_score = 1000;
        gagnant = -1;
        
        # Pour parcourir les éléments de la partie en cours, on utilise l'index composite :
        for (composite_key in joueurs_game) {
            split(composite_key, indices, SUBSEP);
            game_id_in_loop = indices[1];
            j = indices[2];

            if (game_id_in_loop == current_game_id) {
                s = scores_game[game_id_in_loop, j]; # Accès Corrigé

                # Mise à jour du score minimum
                if (s < min_score) {
                    min_score = s;
                    gagnant = j;
                }
                
                parties_jouees[j]++;
                scores_finaux_total[j] += s;
            }
        }

        for (composite_key in joueurs_game) {
             split(composite_key, indices, SUBSEP);
             game_id_in_loop = indices[1];
             j = indices[2];

             if (game_id_in_loop == current_game_id) {
                if (j == gagnant) {
                    parties_gagnees[j]++;
                } else {
                    parties_perdues[j]++;
                }
             }
        }
        
        # Préparation pour la prochaine partie
        current_game_id++;
        entries_in_current_game = 0;
    }
}

END {
    total_parties_analyzed = current_game_id - 1;
    
    # Redirection de la sortie AWK vers le fichier stats_joueurs.txt
    print "## STATISTIQUES JOUEURS (Agrégation sur " total_parties_analyzed " parties) ##" > "stats_joueurs.txt";

    for (j in parties_jouees) {
        if (j == "") {
             continue;
        }

        total_parties_jouees = parties_jouees[j];
        
        # points moyens
        if (total_parties_jouees > 0) {
            pts_moyens = scores_finaux_total[j] / total_parties_jouees;
        } else {
            pts_moyens = 0;
        }

        # Affichage des résultats
        print "JOUEUR " j ":" >> "stats_joueurs.txt";
        printf "  Parties jouées: %d\n", parties_jouees[j] >> "stats_joueurs.txt";
        printf "  Parties gagnées: %d\n", parties_gagnees[j] >> "stats_joueurs.txt";
        printf "  Parties perdues: %d\n", parties_perdues[j] >> "stats_joueurs.txt";
        printf "  Points moyens par partie: %f\n", pts_moyens >> "stats_joueurs.txt";
    }
}