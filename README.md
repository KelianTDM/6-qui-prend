# 6 Qui Prend - Architecture Système & IPC

![Language C](https://img.shields.io/badge/Language-C11-blue) ![IPC](https://img.shields.io/badge/Tech-Named_Pipes-orange) ![Scripting](https://img.shields.io/badge/Scripting-Bash%20%7C%20AWK-green) ![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey)

## 📄 Description
Implémentation système du jeu de société **"6 qui prend"** en langage C. Ce projet universitaire a pour but d'explorer la programmation système sous UNIX, notamment la gestion des processus et la communication inter-processus (IPC) sans bibliothèques de haut niveau.

Le jeu se déroule entièrement dans le terminal. Il orchestre une architecture **Client-Serveur** où le serveur gère la logique du jeu (règles, plateau) et les clients (humains ou bots) prennent des décisions de manière asynchrone.

## ⚙️ Architecture Technique
Le projet repose sur une séparation stricte entre le moteur de jeu et les joueurs :

* **Communication (IPC) :** Utilisation de **Tubes Nommés (FIFOs)**. Chaque joueur dispose de deux canaux dédiés (`client_X_in` et `client_X_out`) pour échanger avec le serveur.
* **Protocole Binaire :** Les échanges se font via des structures binaires définies (`MsgHeader`), garantissant l'intégrité des données (Cartes, Scores, État du plateau).
* **Gestion des Processus :** Utilisation de `fork()` pour instancier les bots et gérer le cycle de vie de la partie.
* **Bots IA :** Implémentation de deux stratégies :
    * *Défaut :* Joue la plus petite carte.
    * *Intelligent :* Analyse les lignes pour minimiser les pénalités ("têtes de bœufs").

## 📊 Pipeline de Données (Logs & Stats)
Le projet inclut une chaîne de traitement de données complète :
1.  **Génération :** Le serveur produit des logs bruts (`scores.log`) pendant la partie.
2.  **Traitement (AWK) :** Le script `stats.awk` parse les logs pour extraire les métriques (victoires, défaites, moyennes).
3.  **Reporting (Bash/LaTeX) :** Le script `stats.sh` automatise la génération d'un rapport PDF via `pdflatex`.

## 🚀 Installation et Utilisation

### Compilation
Un `Makefile` est fourni pour automatiser la compilation.
```bash
make
# Génère les exécutables : serveur, client, bot
