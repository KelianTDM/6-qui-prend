#!/bin/sh

LOG_FILE="scores.log"
STATS_DATA_FILE="stats_joueurs.txt"
OUTPUT_TEX="stats_report.tex"
OUTPUT_PDF="stats_joueurs.pdf"
N_PLAYERS=4 

if [ ! -f "$LOG_FILE" ]; then
    echo "Erreur : Fichier de log $LOG_FILE non trouvé."
    exit 1
fi

echo "Analyse des logs et génération du fichier de données brutes ($STATS_DATA_FILE)..."

awk -F'|' -v N="$N_PLAYERS" -f stats.awk "$LOG_FILE"


if [ ! -f "$STATS_DATA_FILE" ]; then
    echo "Erreur : Le fichier de statistiques brutes ($STATS_DATA_FILE) n'a pas été créé."
    exit 1
fi

echo "Création du fichier LaTeX ($OUTPUT_TEX)..."

cat << EOF > "$OUTPUT_TEX"
\documentclass{article}
\usepackage[utf8]{inputenc}
\usepackage[T1]{fontenc}
\usepackage{fancyvrb}
\title{Statistiques}
\date{\today}

\begin{document}
\maketitle

% Insertion des statistiques formatées par AWK (stats_joueurs.txt)
\VerbatimInput{$STATS_DATA_FILE}

\end{document}
EOF

echo "Compilation du rapport LaTeX en PDF..."

if command -v pdflatex >/dev/null 2>&1; then
    
    pdflatex "$OUTPUT_TEX" > /dev/null
    pdflatex "$OUTPUT_TEX" > /dev/null 

    echo "Succès : Le rapport PDF est disponible sous le nom : $OUTPUT_PDF"

    rm -f *.aux "$OUTPUT_TEX" 
else
    echo "ATTENTION : La commande 'pdflatex' n'a pas été trouvée."
    echo "Le fichier LaTeX non compilé est disponible : $OUTPUT_TEX"
fi