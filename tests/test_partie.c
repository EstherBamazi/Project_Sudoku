#include "partie.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int grille[TAILLE][TAILLE];
    int solution[TAILLE][TAILLE];
    int initiale[TAILLE][TAILLE];
    int copieInitiale[TAILLE][TAILLE];
    int copieSolution[TAILLE][TAILLE];
    const int objectifs[] = {45, 35, 28};

    initialiserGrille(grille);
    assert(compterCasesInitiales(grille) == 0);
    grille[0][0] = 1;
    grille[TAILLE / 2][TAILLE / 2] = 5;
    grille[TAILLE - 1][TAILLE - 1] = 9;
    copierGrille(copieInitiale, grille);
    assert(compterCasesInitiales(grille) == 3);
    assert(memcmp(grille, copieInitiale, sizeof grille) == 0);
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            grille[i][j] = 1;
        }
    }
    /* Ce compteur compte les cases remplies, sans juger la validite Sudoku. */
    assert(compterCasesInitiales(grille) == TAILLE * TAILLE);

    for (int niveau = 1; niveau <= 3; niveau++) {
        srand((unsigned int)(100 + niveau));
        assert(preparerPartie(solution, grille, initiale, niveau) == 1);
        assert(grilleComplete(solution));
        assert(!grilleComplete(grille));
        assert(memcmp(grille, initiale, sizeof grille) == 0);
        assert(compterCasesInitiales(initiale) == objectifs[niveau - 1]);
        copierGrille(copieInitiale, initiale);
        copierGrille(copieSolution, solution);
        assert(compterSolutions(grille) == 1);
        assert(memcmp(grille, copieInitiale, sizeof grille) == 0);

        int ligneVide = -1;
        int colonneVide = -1;
        for (int i = 0; i < TAILLE; i++) {
            for (int j = 0; j < TAILLE; j++) {
                if (initiale[i][j] != 0) {
                    assert(initiale[i][j] == solution[i][j]);
                } else {
                    ligneVide = i;
                    colonneVide = j;
                }
            }
        }
        assert(ligneVide >= 0 && colonneVide >= 0);
        /* Jouer ne doit modifier ni les indices initiaux ni la solution. */
        assert(jouerCoup(grille, initiale, solution, ligneVide, colonneVide,
                         solution[ligneVide][colonneVide]) == 1);
        assert(memcmp(initiale, copieInitiale, sizeof initiale) == 0);
        assert(memcmp(solution, copieSolution, sizeof solution) == 0);
        assert(compterCasesInitiales(grille) == objectifs[niveau - 1] + 1);
        assert(compterCasesInitiales(initiale) == objectifs[niveau - 1]);
    }

    assert(preparerPartie(solution, grille, initiale, -1) == 0);
    assert(preparerPartie(solution, grille, initiale, 0) == 0);
    assert(preparerPartie(solution, grille, initiale, 4) == 0);
    puts("OK : preparation des 3 niveaux, copies independantes, indices, comptage et niveaux invalides.");
    return 0;
}
