#ifndef JOUEUR_H
#define JOUEUR_H

#include "partie.h"

// Prototypes pour la gestion des saisies et de l'interface
int lireLigne(char texte[], int capacite);
int lireEntiers(char *texte, int valeurs[], int nombre);
int commande(char *texte, int lettre);

// Lancement de l'interface et de la boucle de jeu
void lancerBoucleJeu(void);

#endif
