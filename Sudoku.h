#ifndef SUDOKU_H
#define SUDOKU_H

#define N 3
#define TAILLE (N * N)

/* 0 represente une case vide. Coordonnees internes : 0 a 8. */
void initialiserGrille(int grille[TAILLE][TAILLE]);
void copierGrille(int destination[TAILLE][TAILLE], int source[TAILLE][TAILLE]);
int placementPossible(int grille[TAILLE][TAILLE], int ligne, int colonne, int valeur);
int grilleValide(int grille[TAILLE][TAILLE]);
int grilleComplete(int grille[TAILLE][TAILLE]);
int resoudreGrille(int grille[TAILLE][TAILLE]);
/* Compte jusqu'a 2 et restaure toujours la grille recue. */
int compterSolutions(int grille[TAILLE][TAILLE]);
/* niveaux 1, 2, 3 : 45, 35, 28 indices. Retourne 0 si echec. */
int genererPartie(int solution[TAILLE][TAILLE], int grille[TAILLE][TAILLE], int niveau);
/* Retour : 1 accepte, 0 faux chiffre, -1 coordonnees/valeur, -2 case initiale. */
int jouerCoup(int grille[TAILLE][TAILLE], int initiale[TAILLE][TAILLE],
              int solution[TAILLE][TAILLE], int ligne, int colonne, int valeur);
void afficherGrille(int grille[TAILLE][TAILLE]);

#endif

