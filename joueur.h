#ifndef JOUEUR_H
#define JOUEUR_H

/* Interface console, separee du moteur selon la proposition de Rayan. */
int lireLigne(char texte[], int capacite);
int lireEntiers(char texte[], int valeurs[], int nombre);
int commande(char texte[], char minuscule, char majuscule);
int lancerBoucleJeu(void);

#endif
