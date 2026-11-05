#include "joueur.h"
#include <stdlib.h>
#include <time.h>

int main(void)
{
    // Initialisation du générateur de nombres aléatoires
    srand((unsigned int)time(NULL));

    // Lancement du jeu Sudoku
    lancerBoucleJeu();

    return 0;
}