#include "Sudoku.h"
#include <stdio.h>
#include <stdlib.h>

void initialiserGrille(int grille[TAILLE][TAILLE])
{
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            grille[i][j] = 0;
        }
    }
}

void copierGrille(int destination[TAILLE][TAILLE], int source[TAILLE][TAILLE])
{
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            destination[i][j] = source[i][j];
        }
    }
}

int placementPossible(int grille[TAILLE][TAILLE], int ligne, int colonne, int valeur)
{
    if (ligne < 0 || ligne >= TAILLE || colonne < 0 || colonne >= TAILLE ||
        valeur < 1 || valeur > TAILLE) {
        return 0;
    }
    for (int k = 0; k < TAILLE; k++) {
        /* Ignorer la case elle-meme permet aussi de verifier une grille remplie. */
        if (k != colonne && grille[ligne][k] == valeur) {
            return 0;
        }
        if (k != ligne && grille[k][colonne] == valeur) {
            return 0;
        }
    }
    /* Division entiere : les indices 0,1,2 donnent 0 ; 3,4,5 donnent 3. */
    int debutLigne = (ligne / N) * N;
    int debutColonne = (colonne / N) * N;
    for (int i = debutLigne; i < debutLigne + N; i++) {
        for (int j = debutColonne; j < debutColonne + N; j++) {
            if ((i != ligne || j != colonne) && grille[i][j] == valeur) {
                return 0;
            }
        }
    }
    return 1;
}

int grilleValide(int grille[TAILLE][TAILLE])
{
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (grille[i][j] != 0 && !placementPossible(grille, i, j, grille[i][j])) {
                return 0;
            }
        }
    }
    return 1;
}

int grilleComplete(int grille[TAILLE][TAILLE])
{
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (grille[i][j] == 0) {
                return 0;
            }
        }
    }
    return grilleValide(grille);
}

/* Placer un element tire au hasard a la fin, puis recommencer avant lui.
   Ce melange par echanges est aussi appele Fisher-Yates. */
void melanger(int valeurs[], int longueur)
{
    for (int i = longueur - 1; i > 0; i--) {
        /* Le reste de la division est compris entre 0 et i. */
        int j = rand() % (i + 1);
        int temporaire = valeurs[i];
        valeurs[i] = valeurs[j];
        valeurs[j] = temporaire;
    }
}

/* Parcourir les lignes puis les colonnes et choisir la premiere case vide.
   Retour : -1 grille complete, 0 impasse, sinon nombre de candidats. */
int choisirCase(int grille[TAILLE][TAILLE], int *ligne, int *colonne,
                       int candidats[TAILLE])
{
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (grille[i][j] != 0) {
                continue;
            }
            /* Les pointeurs permettent de transmettre les deux coordonnees. */
            *ligne = i;
            *colonne = j;
            int nombre = 0;
            for (int v = 1; v <= TAILLE; v++) {
                if (placementPossible(grille, i, j, v)) {
                    candidats[nombre] = v;
                    nombre++;
                }
            }
            return nombre;
        }
    }
    return -1;
}

int resoudre(int grille[TAILLE][TAILLE], int aleatoire)
{
    int ligne = 0, colonne = 0, candidats[TAILLE];
    int nombre = choisirCase(grille, &ligne, &colonne, candidats);
    if (nombre == -1) {
        return 1; /* Aucune case vide : la solution est complete. */
    }
    if (aleatoire) {
        melanger(candidats, nombre);
    }
    for (int k = 0; k < nombre; k++) {
        grille[ligne][colonne] = candidats[k];
        if (resoudre(grille, aleatoire)) {
            return 1; /* Garder la solution trouvee. */
        }
        grille[ligne][colonne] = 0; /* Retour en arriere. */
    }
    return 0;
}

int resoudreGrille(int grille[TAILLE][TAILLE])
{
    if (!grilleValide(grille)) {
        return 0;
    }
    return resoudre(grille, 0);
}

int compter(int grille[TAILLE][TAILLE], int limite)
{
    int ligne = 0, colonne = 0, candidats[TAILLE];
    int nombre = choisirCase(grille, &ligne, &colonne, candidats);
    if (nombre == -1) {
        return 1;
    }
    int total = 0;
    for (int k = 0; k < nombre && total < limite; k++) {
        grille[ligne][colonne] = candidats[k];
        /* Exemple : limite=2, total=1 => chercher encore une seule solution. */
        int restantes = limite - total;
        int trouvees = compter(grille, restantes);
        total = total + trouvees;
        /* Toujours annuler, meme apres un succes : on compte sans changer la grille. */
        grille[ligne][colonne] = 0;
    }
    return total;
}

int compterSolutions(int grille[TAILLE][TAILLE])
{
    if (!grilleValide(grille)) {
        return 0;
    }
    return compter(grille, 2);
}

int genererPartie(int solution[TAILLE][TAILLE], int grille[TAILLE][TAILLE], int niveau)
{
    const int objectifs[] = {45, 35, 28};
    const int minima[] = {4, 3, 2};
    if (niveau < 1 || niveau > 3) {
        return 0;
    }
    int objectif = objectifs[niveau - 1], minimum = minima[niveau - 1];
    /* Nombre de tentatives borne : aucune boucle infinie de regeneration. */
    for (int essai = 0; essai < 100; essai++) {
        initialiserGrille(solution);
        if (!resoudre(solution, 1)) {
            return 0;
        }
        copierGrille(grille, solution);
        int positions[TAILLE * TAILLE];
        int lignes[TAILLE], colonnes[TAILLE], blocs[TAILLE];
        for (int k = 0; k < TAILLE; k++) {
            lignes[k] = TAILLE;
            colonnes[k] = TAILLE;
            blocs[k] = TAILLE;
        }
        for (int k = 0; k < TAILLE * TAILLE; k++) {
            positions[k] = k;
        }
        melanger(positions, TAILLE * TAILLE);
        int restants = TAILLE * TAILLE;
        for (int k = 0; k < TAILLE * TAILLE && restants > objectif; k++) {
            int i = positions[k] / TAILLE, j = positions[k] % TAILLE;
            int bloc = (i / N) * N + j / N;
            if (lignes[i] <= minimum || colonnes[j] <= minimum || blocs[bloc] <= minimum) {
                continue;
            }
            int ancienneValeur = grille[i][j];
            grille[i][j] = 0;
            if (compterSolutions(grille) == 1) {
                lignes[i]--;
                colonnes[j]--;
                blocs[bloc]--;
                restants--;
            } else {
                grille[i][j] = ancienneValeur;
            }
        }
        if (restants == objectif) {
            return 1;
        }
    }
    initialiserGrille(grille);
    initialiserGrille(solution);
    return 0;
}

int jouerCoup(int grille[TAILLE][TAILLE], int initiale[TAILLE][TAILLE],
              int solution[TAILLE][TAILLE], int ligne, int colonne, int valeur)
{
    if (ligne < 0 || ligne >= TAILLE || colonne < 0 || colonne >= TAILLE ||
        valeur < 1 || valeur > TAILLE) {
        return -1;
    }
    if (initiale[ligne][colonne] != 0) {
        return -2;
    }
    if (solution[ligne][colonne] != valeur) {
        return 0;
    }
    grille[ligne][colonne] = valeur;
    return 1;
}

void afficherGrille(int grille[TAILLE][TAILLE])
{
    printf("    1 2 3   4 5 6   7 8 9\n");
    for (int i = 0; i < TAILLE; i++) {
        if (i % N == 0) {
            printf("  +-------+-------+-------+\n");
        }
        printf("%d ", i + 1);
        for (int j = 0; j < TAILLE; j++) {
            if (j % N == 0) {
                printf("| ");
            }
            if (grille[i][j] == 0) {
                printf(". ");
            } else {
                printf("%d ", grille[i][j]);
            }
        }
        printf("|\n");
    }
    printf("  +-------+-------+-------+\n");
}

