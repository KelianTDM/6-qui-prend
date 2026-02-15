# Makefile pour le projet jeu6qp

# Compilateur
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Dossiers
OBJDIR = build

# Fichiers commun
COMMON_SRC = card.c line.c deck.c

#Fichiers serveur
SERVEUR_SRC =  game.c player.c serveur.c bot.c

#Fichiers client
CLIENT_SRC = client.c

#Fichiers bot
BOT_SRC = bot.c

# Génération automatique des .o dans build/
COMMON_OBJ = $(addprefix $(OBJDIR)/,$(COMMON_SRC:.c=.o))
SERVEUR_OBJ = $(addprefix $(OBJDIR)/,$(SERVEUR_SRC:.c=.o))
CLIENT_OBJ = $(addprefix $(OBJDIR)/,$(CLIENT_SRC:.c=.o))
BOT_OBJ = $(addprefix $(OBJDIR)/,$(BOT_SRC:.c=.o))


# Règle par défaut
all: serveur client bot

# Création du dossier build s'il n'existe pas
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Règle pour compiler chaque .c en .o dans build/
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compilation finale serveur
serveur: $(COMMON_OBJ) $(SERVEUR_OBJ)
	$(CC) $^ -o serveur

# Compilation finale client
client: $(COMMON_OBJ) $(CLIENT_OBJ)
	$(CC) $^ -o client

# Compilation finale bot
bot: $(COMMON_OBJ) $(BOT_OBJ)
	$(CC) $^ -o bot

# Nettoyage des fichiers générés
clean:
	rm -rf $(OBJDIR)
	rm -f serveur client


.PHONY: all serveur client clean