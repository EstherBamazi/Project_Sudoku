#ifndef PARTIE_H
#define PARTIE_H

#include "Sudoku.h"

/* Genere les deux grilles puis conserve les cases initiales. 1 = succes. */
int preparerPartie(int solution[TAILLE][TAILLE], int grille[TAILLE][TAILLE],
                  int initiale[TAILLE][TAILLE], int niveau);
int compterCasesInitiales(int grille[TAILLE][TAILLE]);
/* 9, 5 ou 3 erreurs pour les niveaux 1, 2 ou 3. 0 si niveau invalide. */
int limiteErreurs(int niveau);

#endif
