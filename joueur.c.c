#include "joueur.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Lit une ligne complete, en rejetant proprement les lignes trop longues. */
int lireLigne(char texte[], int capacite)
{
    if (!fgets(texte, capacite, stdin))
        return 0;

    if (!strchr(texte, '\n') && !feof(stdin))
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        texte[0] = '\0';
    }

    return 1;
}

/* Accepte exactement 'nombre' entiers entre 1 et 9, separes par des espaces. */
int lireEntiers(char *texte, int valeurs[], int nombre)
{
    for (int i = 0; i < nombre; i++)
    {
        while (isspace((unsigned char)*texte))
            texte++;

        char *fin;
        errno = 0;
        long valeur = strtol(texte, &fin, 10);

        if (fin == texte || errno || valeur < 1 || valeur > 9)
            return 0;

        if (*fin && !isspace((unsigned char)*fin))
            return 0;

        valeurs[i] = (int)valeur;
        texte = fin;
    }

    while (isspace((unsigned char)*texte))
        texte++;

    return *texte == '\0';
}

/* Verifie une commande mono-lettre comme 'q' ou 'o'. */
int commande(char *texte, int lettre)
{
    while (isspace((unsigned char)*texte))
        texte++;

    if (tolower((unsigned char)*texte) != lettre)
        return 0;

    texte++;

    while (isspace((unsigned char)*texte))
        texte++;

    return *texte == '\0';
}

/* Boucle de jeu principale gerant l'interaction avec le joueur */
void lancerBoucleJeu(void)
{
    int solution[TAILLE][TAILLE];
    int grille[TAILLE][TAILLE];
    int initiale[TAILLE][TAILLE];

    char texte[128];
    int choix[1];
    int coup[3];

    puts("=========================================");
    puts("         BIENVENUE AU JEU SUDOKU         ");
    puts("=========================================");
    puts("Chiffres de 1 a 9, sans doublon par ligne, colonne et bloc.");
    puts("Les cases initiales sont protegees.");
    puts("Un faux chiffre coute une erreur.");

    /* Choix de la difficulte */
    for (;;)
    {
        printf("\nNiveau : 1 facile, 2 moyen, 3 difficile : ");
        fflush(stdout);

        if (!lireLigne(texte, sizeof texte))
            return;

        if (lireEntiers(texte, choix, 1) && choix[0] <= 3)
            break;

        puts("Veuillez choisir 1, 2 ou 3.");
    }

    /* Preparation de la partie */
    puts("\nGeneration de la grille en cours...");

    if (!preparerPartie(solution, grille, initiale, choix[0]))
    {
        puts("Generation impossible apres 100 tentatives. Relancez le programme.");
        return;
    }

    printf("Nombre de cases initiales : %d\n", compterCasesInitiales(initiale));

    /* Seuil de limites d'erreurs selon la difficulte */
    const int limites[] = {9, 5, 3};
    int erreurs = 0;
    int limite = limites[choix[0] - 1];

    /* Boucle de jeu */
    while (!grilleComplete(grille))
    {
        afficherGrille(grille);

        printf("\nErreurs : %d/%d. ", erreurs, limite);
        printf("Entrer 'ligne colonne valeur' (ex. 2 4 7), ou 'q' pour abandonner : ");
        fflush(stdout);

        if (!lireLigne(texte, sizeof texte))
        {
            puts("\nFin de saisie.");
            return;
        }

        /* Abandon de la partie */
        if (commande(texte, 'q'))
        {
            printf("Confirmer l'abandon ? (o/n) : ");
            fflush(stdout);

            if (!lireLigne(texte, sizeof texte))
                return;

            if (commande(texte, 'o'))
            {
                puts("\n=========================================");
                puts("            PARTIE ABANDONNEE            ");
                puts("=========================================");
                puts("Voici la solution de la grille :");
                afficherGrille(solution);
                return;
            }

            continue;
        }

        /* Lecture de la saisie */
        if (!lireEntiers(texte, coup, 3))
        {
            puts("Saisie invalide ! Entrer exactement trois entiers de 1 a 9.");
            continue;
        }

        /* Verification du coup */
        int resultat = jouerCoup(
            grille,
            initiale,
            solution,
            coup[0] - 1,
            coup[1] - 1,
            coup[2]
        );

        if (resultat == -2)
        {
            puts(">> Case protegee ! Vous ne pouvez pas modifier une case initiale.");
        }
        else if (resultat == -1)
        {
            puts(">> Coordonnees ou valeur invalides.");
        }
        else if (resultat == 0)
        {
            erreurs++;
            puts(">> Incorrect ! Ce chiffre ne correspond pas a la solution.");

            /* Condition de défaite par limite d'erreurs */
            if (erreurs >= limite)
            {
                puts("\n=========================================");
                puts("                DEFAITE !                ");
                puts("=========================================");
                puts("Limite d'erreurs atteinte.");
                puts("Voici la solution de la grille :");
                afficherGrille(solution);
                return;
            }
        }
        else if (resultat == 1)
        {
            puts(">> Coup correct !");
        }
    }

    /* Condition de victoire */
    afficherGrille(grille);
    puts("\n=========================================");
    puts("   FELICITATIONS ! VOUS AVEZ GAGNE !     ");
    puts("=========================================");
    puts("La grille est complete et totalement valide.");
}
