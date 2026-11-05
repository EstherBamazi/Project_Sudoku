#include "joueur.h"
#include "partie.h"
#include <stdio.h>

/* Espace, tabulation ou retour chariot (fins de ligne Windows). */
int estEspace(char caractere)
{
    if (caractere == ' ' || caractere == '\t' || caractere == '\r') {
        return 1;
    }
    return 0;
}

/* Retour : 1 = ligne lue, 0 = fin de saisie, -1 = ligne refusee. */
int lireLigne(char texte[], int capacite)
{
    if (capacite < 1) {
        return -1;
    }
    int position = 0;
    int invalide = 0;
    /* getchar renvoie un int : il faut pouvoir reconnaitre EOF. */
    int caractere = getchar();
    if (caractere == EOF) {
        return 0;
    }

    while (caractere != '\n' && caractere != EOF) {
        if (caractere == '\0') {
            invalide = 1; /* Ne pas accepter une fin de chaine cachee. */
        }
        if (position < capacite - 1) {
            texte[position] = (char)caractere;
            position++;
        } else {
            invalide = 1;
        }
        /* Meme si la ligne est trop longue, lire jusqu'a sa fin. */
        caractere = getchar();
    }
    texte[position] = '\0';
    if (invalide) {
        texte[0] = '\0';
        return -1;
    }
    return 1;
}

/* Accepte exactement nombre chiffres de 1 a 9 separes par des espaces. */
int lireEntiers(char texte[], int valeurs[], int nombre)
{
    int position = 0;
    for (int i = 0; i < nombre; i++) {
        while (estEspace(texte[position])) {
            position++;
        }
        if (texte[position] < '1' || texte[position] > '9') {
            return 0;
        }
        /* Exemple : le caractere '7' devient l'entier 7. */
        valeurs[i] = texte[position] - '0';
        position++;
        if (texte[position] != '\0' && !estEspace(texte[position])) {
            return 0;
        }
    }
    while (estEspace(texte[position])) {
        position++;
    }
    if (texte[position] != '\0') {
        return 0;
    }
    return 1;
}

/* Accepte la commande seule, en minuscule ou en majuscule. */
int commande(char texte[], char minuscule, char majuscule)
{
    int position = 0;
    while (estEspace(texte[position])) {
        position++;
    }
    if (texte[position] != minuscule && texte[position] != majuscule) {
        return 0;
    }
    position++;
    while (estEspace(texte[position])) {
        position++;
    }
    if (texte[position] != '\0') {
        return 0;
    }
    return 1;
}

int lancerBoucleJeu(void)
{
    int solution[TAILLE][TAILLE], grille[TAILLE][TAILLE], initiale[TAILLE][TAILLE];
    char texte[128];
    int choix[1], coup[3];
    puts("SUDOKU : chiffres 1 a 9, sans doublon par ligne, colonne et bloc.");
    puts("Les cases initiales sont protegees. Un faux chiffre coute une erreur.");
    puts("Une saisie mal formee ou une case protegee ne coute aucune erreur.");
    int niveauValide = 0;
    while (!niveauValide) {
        printf("Niveau : 1 facile, 2 moyen, 3 difficile : ");
        fflush(stdout);
        int lecture = lireLigne(texte, sizeof texte);
        if (lecture == 0) {
            return 0;
        }
        if (lecture == 1 && lireEntiers(texte, choix, 1) && choix[0] <= 3) {
            niveauValide = 1;
        } else {
            puts("Choisir 1, 2 ou 3.");
        }
    }
    puts("Generation en cours...");
    if (!preparerPartie(solution, grille, initiale, choix[0])) {
        puts("Generation impossible apres 100 tentatives. Relancez le programme.");
        return 1;
    }
    printf("Nombre de cases initiales : %d\n", compterCasesInitiales(initiale));
    int erreurs = 0, limite = limiteErreurs(choix[0]);
    while (!grilleComplete(grille)) {
        afficherGrille(grille);
        printf("Erreurs : %d/%d. Entrer ligne colonne valeur sur une seule ligne (ex. 2 4 7), ou q : ", erreurs, limite);
        fflush(stdout);
        int lecture = lireLigne(texte, sizeof texte);
        if (lecture == 0) {
            puts("\nFin de saisie.");
            return 0;
        }
        if (commande(texte, 'q', 'Q')) {
            printf("Confirmer l'abandon ? o/n : ");
            fflush(stdout);
            if (lireLigne(texte, sizeof texte) == 0) {
                return 0;
            }
            if (commande(texte, 'o', 'O')) {
                puts("Partie abandonnee.");
                puts("Voici la solution :");
                afficherGrille(solution);
                return 0;
            }
            continue;
        }
        if (!lireEntiers(texte, coup, 3)) {
            puts("Entrer trois chiffres de 1 a 9 separes par des espaces, puis Entree.");
            continue;
        }
        int resultat = jouerCoup(grille, initiale, solution, coup[0] - 1, coup[1] - 1, coup[2]);
        if (resultat == -2) {
            puts("Cette case initiale est protegee.");
        } else if (resultat == -1) {
            puts("Coordonnees ou valeur invalides.");
        } else if (resultat == 0) {
            erreurs++;
            puts("Ce chiffre ne correspond pas a la solution.");
            if (erreurs >= limite) {
                puts("Defaite : limite d'erreurs atteinte.");
                puts("Voici la solution :");
                afficherGrille(solution);
                return 0;
            }
        }
    }
    afficherGrille(grille);
    puts("Victoire ! La grille est complete et valide.");
    return 0;
}

