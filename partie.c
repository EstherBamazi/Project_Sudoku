#include "partie.h"

int preparerPartie(int solution[TAILLE][TAILLE],
int grille[TAILLE][TAILLE],
int initiale[TAILLE][TAILLE],
int niveau)
{
// 1. Generer la solution et la grille jouable
if (!genererPartie(solution, grille, niveau))
{
return 0;
}


// 2. Sauvegarder la grille initiale
copierGrille(initiale, grille);

// 3. Preparation reussie
return 1;


}

int compterCasesInitiales(int grille[TAILLE][TAILLE])
{
    int compteur = 0;

    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE; j++)
        {
            if (grille[i][j] != 0)
            {
                compteur++;
            }
        }
    }

    return compteur;
}