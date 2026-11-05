#include "Sudoku.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int g[9][9], s[9][9], copie[9][9], initiale[9][9];
    initialiserGrille(g);
    assert(compterSolutions(g) == 2);
    for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) assert(g[i][j] == 0);
    g[0][0] = 1; g[0][1] = 1;
    assert(!grilleValide(g) && compterSolutions(g) == 0 && !resoudreGrille(g));
    initialiserGrille(g);
    for (int j = 0; j < 8; j++) g[0][j] = j + 1;
    g[1][8] = 9; /* Pas de doublon, mais aucun candidat en (0,8). */
    copierGrille(copie, g);
    assert(grilleValide(g) && compterSolutions(g) == 0 && !resoudreGrille(g));
    assert(memcmp(g, copie, sizeof g) == 0);
    assert(!placementPossible(g, -1, 0, 1));
    assert(!genererPartie(s, g, 0));
    const int objectifs[] = {45, 35, 28}, minima[] = {4, 3, 2};
    for (int seed = 0; seed < 30; seed++) {
        for (int niveau = 1; niveau <= 3; niveau++) {
            srand((unsigned int)(seed * 3 + niveau));
            assert(genererPartie(s, g, niveau));
            assert(grilleComplete(s));
            /* Verification independante : chaque unite contient 1..9. */
            for (int i = 0; i < 9; i++) {
                int ligne = 0, colonne = 0, bloc = 0;
                for (int j = 0; j < 9; j++) {
                    ligne |= 1 << s[i][j]; colonne |= 1 << s[j][i];
                    bloc |= 1 << s[(i / 3) * 3 + j / 3][(i % 3) * 3 + j % 3];
                }
                assert(ligne == 1022 && colonne == 1022 && bloc == 1022);
            }
            copierGrille(initiale, g);
            assert(compterSolutions(g) == 1);
            assert(memcmp(g, initiale, sizeof g) == 0);
            int indices = 0;
            for (int i = 0; i < 9; i++) {
                int ligne = 0, colonne = 0, bloc = 0;
                for (int j = 0; j < 9; j++) {
                    ligne += g[i][j] != 0; colonne += g[j][i] != 0;
                    bloc += g[(i / 3) * 3 + j / 3][(i % 3) * 3 + j % 3] != 0;
                    if (g[i][j]) { indices++; assert(g[i][j] == s[i][j]); }
                }
                assert(ligne >= minima[niveau - 1] && colonne >= minima[niveau - 1]
                       && bloc >= minima[niveau - 1]);
            }
            assert(indices == objectifs[niveau - 1]);
            copierGrille(copie, g);
            assert(resoudreGrille(copie));
            assert(memcmp(copie, s, sizeof s) == 0);
            assert(jouerCoup(g, initiale, s, 9, 0, 1) == -1);
            for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) {
                if (initiale[i][j]) assert(jouerCoup(g, initiale, s, i, j, s[i][j]) == -2);
                else {
                    assert(jouerCoup(g, initiale, s, i, j, s[i][j] % 9 + 1) == 0);
                    assert(g[i][j] == 0);
                    assert(jouerCoup(g, initiale, s, i, j, s[i][j]) == 1);
                }
            }
            assert(grilleComplete(g));
        }
    }
    puts("OK : 90 parties, validite, unicite, restauration, niveaux, repartition, coups et victoire.");
    return 0;
}

