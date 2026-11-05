#include "joueur.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Cette partie existe seulement si CMake active SDL2. */
#ifdef SUDOKU_AVEC_SDL
#include "interfaceSDL.h"
#endif

int main(int argc, char *argv[])
{
    /* Initialisation unique, commune aux deux interfaces. */
    srand((unsigned int)time(NULL));
    if (argc == 2 && strcmp(argv[1], "--console") == 0) {
        return lancerBoucleJeu();
    }
    if (argc != 1) {
        puts("Utilisation : sudoku [--console]");
        return 1;
    }
#ifdef SUDOKU_AVEC_SDL
    return lancerInterfaceSDL();
#else
    return lancerBoucleJeu();
#endif
}
