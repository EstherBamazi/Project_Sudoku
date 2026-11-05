#ifndef INTERFACE_SDL_H
#define INTERFACE_SDL_H

/* Ouvre la fenetre SDL2. Retourne 0 a la fermeture, 1 en cas d'erreur SDL.
   Le moteur et les regles de jeu restent dans Sudoku.c et partie.c. */
int lancerInterfaceSDL(void);

#endif
