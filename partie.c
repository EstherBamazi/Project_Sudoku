#include "partie.h"

/* Contribution de Cedric : preparation commune aux deux interfaces. */
int preparerPartie(int solution[TAILLE][TAILLE], int grille[TAILLE][TAILLE],
                  int initiale[TAILLE][TAILLE], int niveau)
{
    if (!genererPartie(solution, grille, niveau)) {
        return 0;
    }
    copierGrille(initiale, grille);
    return 1;
}

int compterCasesInitiales(int grille[TAILLE][TAILLE])
{
    int compteur = 0;
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (grille[i][j] != 0) {
                compteur++;
            }
        }
    }
    return compteur;
}

int limiteErreurs(int niveau)
{
    if (niveau == 1) {
        return 9;
    }
    if (niveau == 2) {
        return 5;
    }
    if (niveau == 3) {
        return 3;
    }
    return 0;
}
