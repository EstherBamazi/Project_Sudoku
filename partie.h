#ifndef PARTIE_H
#define PARTIE_H

#include "Sudoku.h"

int preparerPartie(int solution[TAILLE][TAILLE],
                    int grille[TAILLE][TAILLE],
                    int initiale[TAILLE][TAILLE],
                    int niveau);
int compterCasesInitiales(int grille[TAILLE][TAILLE]);

#endif