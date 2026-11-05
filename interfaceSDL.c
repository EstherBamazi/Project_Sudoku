#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "interfaceSDL.h"
#include "Sudoku.h"
#include "partie.h"
#include <stdio.h>

/* Extension graphique : SDL2 est autorisee par le sujet, mais son API est
   un ajout au cours. SDL_Window et SDL_Renderer sont des objets fournis par
   SDL ; leurs pointeurs permettent aux fonctions SDL d'agir sur la fenetre.
   SDL_Event est une union : on lit key, button ou window selon son type.

   References officielles SDL2 consultees pour cette implementation :
   https://wiki.libsdl.org/SDL2/SDL_CreateWindow
   https://wiki.libsdl.org/SDL2/SDL_CreateRenderer
   https://wiki.libsdl.org/SDL2/SDL_PollEvent
   https://wiki.libsdl.org/SDL2/SDL_RenderSetLogicalSize
   https://wiki.libsdl.org/SDL2/SDL_RenderFillRect
   https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
*/

#define LARGEUR_GUI 1100
#define HAUTEUR_GUI 760
#define GRILLE_X 50
#define GRILLE_Y 142
#define CASE_GUI 60

/* Chaque caractere possede 7 lignes de 5 pixels. Un entier entre 0 et 31
   code une ligne : 31 = 11111 en binaire, donc cinq pixels allumes.
   Exemple : 14 = 01110 represente les trois pixels centraux.
   Des divisions et des restes suffisent pour lire ces pixels.
   Cette petite police interne evite SDL_ttf et un fichier de police externe. */
const char alphabetGUI[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:./-!?+";
const int pixelsGUI[][7] = {
    {14,17,17,31,17,17,17}, {30,17,17,30,17,17,30},
    {14,17,16,16,16,17,14}, {30,17,17,17,17,17,30},
    {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
    {14,17,16,23,17,17,15}, {17,17,17,31,17,17,17},
    {31,4,4,4,4,4,31}, {7,2,2,2,18,18,12},
    {17,18,20,24,20,18,17}, {16,16,16,16,16,16,31},
    {17,27,21,21,17,17,17}, {17,25,21,19,17,17,17},
    {14,17,17,17,17,17,14}, {30,17,17,30,16,16,16},
    {14,17,17,17,21,18,13}, {30,17,17,30,20,18,17},
    {15,16,16,14,1,1,30}, {31,4,4,4,4,4,4},
    {17,17,17,17,17,17,14}, {17,17,17,17,17,10,4},
    {17,17,17,21,21,21,10}, {17,17,10,4,10,17,17},
    {17,17,10,4,4,4,4}, {31,1,2,4,8,16,31},
    {14,17,19,21,25,17,14}, {4,12,4,4,4,4,14},
    {14,17,1,2,4,8,31}, {30,1,1,14,1,1,30},
    {2,6,10,18,31,2,2}, {31,16,16,30,1,1,30},
    {14,16,16,30,17,17,14}, {31,1,2,4,8,8,8},
    {14,17,17,14,17,17,14}, {14,17,17,15,1,1,14},
    {0,4,4,0,4,4,0}, {0,0,0,0,0,6,6},
    {1,1,2,4,8,16,16}, {0,0,0,31,0,0,0},
    {4,4,4,4,4,0,4}, {14,17,1,2,4,0,4},
    {0,4,4,31,4,4,0}
};

/* Palette : les indices sont sombres, les reponses du joueur sont vertes. */
const SDL_Color fondGUI = {239,243,232,255};
const SDL_Color encreGUI = {35,54,41,255};
const SDL_Color vertGUI = {45,106,79,255};
const SDL_Color blancGUI = {255,255,255,255};
const SDL_Color discretGUI = {100,117,104,255};
const SDL_Color rougeGUI = {165,52,50,255};
const SDL_Color pastelGUI = {220,232,214,255};

void rectangleGUI(SDL_Renderer *rendu, int x, int y, int largeur, int hauteur,
                  SDL_Color couleur)
{
    SDL_Rect zone = {x,y,largeur,hauteur};
    SDL_SetRenderDrawColor(rendu,couleur.r,couleur.g,couleur.b,couleur.a);
    SDL_RenderFillRect(rendu,&zone);
}

int longueurTexteGUI(const char texte[])
{
    int longueur = 0;
    while (texte[longueur] != '\0') longueur++;
    return longueur;
}

void texteGUI(SDL_Renderer *rendu, const char texte[], int x, int y,
              int taille, SDL_Color couleur)
{
    for (int k = 0; texte[k] != '\0'; k++) {
        int lettre = 0;
        while (alphabetGUI[lettre] != '\0' && alphabetGUI[lettre] != texte[k]) lettre++;
        if (alphabetGUI[lettre] != '\0') {
            for (int ligne = 0; ligne < 7; ligne++) {
                int diviseur = 16;
                for (int colonne = 0; colonne < 5; colonne++) {
                    if ((pixelsGUI[lettre][ligne] / diviseur) % 2 == 1) {
                        rectangleGUI(rendu,x + colonne * taille,y + ligne * taille,
                                     taille,taille,couleur);
                    }
                    diviseur = diviseur / 2;
                }
            }
        }
        x = x + 6 * taille; /* Laisser un pixel vide entre les lettres. */
    }
}

void nombreGUI(SDL_Renderer *rendu, int valeur, int x, int y, int taille,
               SDL_Color couleur)
{
    char texte[3];
    if (valeur >= 10) {
        texte[0] = (char)('0' + valeur / 10);
        texte[1] = (char)('0' + valeur % 10);
        texte[2] = '\0';
    } else {
        texte[0] = (char)('0' + valeur);
        texte[1] = '\0';
    }
    texteGUI(rendu,texte,x,y,taille,couleur);
}

int dansZoneGUI(int x, int y, int gauche, int haut, int largeur, int hauteur)
{
    return x >= gauche && x < gauche + largeur && y >= haut && y < haut + hauteur;
}

void boutonGUI(SDL_Renderer *rendu, const char texte[], int x, int y, int largeur,
               int hauteur, int sourisX, int sourisY, int principal)
{
    SDL_Color couleur = pastelGUI;
    SDL_Color lettres = encreGUI;
    if (principal || dansZoneGUI(sourisX,sourisY,x,y,largeur,hauteur)) {
        couleur = vertGUI;
        lettres = blancGUI;
    }
    rectangleGUI(rendu,x,y,largeur,hauteur,couleur);
    int taille = 2;
    int texteX = x + (largeur - longueurTexteGUI(texte) * 6 * taille + taille) / 2;
    texteGUI(rendu,texte,texteX,y + (hauteur - 7 * taille) / 2,taille,lettres);
}

void menuGUI(SDL_Renderer *rendu, int sourisX, int sourisY, const char message[])
{
    texteGUI(rendu,"SUDOKU",335,78,12,encreGUI);
    texteGUI(rendu,"UNE GRILLE. TROIS NIVEAUX.",310,190,3,vertGUI);
    texteGUI(rendu,"CHOISISSEZ VOTRE PARTIE",352,260,3,encreGUI);
    texteGUI(rendu,"COMMANDES",65,330,2,vertGUI);
    texteGUI(rendu,"CLIC : UNE CASE",65,372,2,discretGUI);
    texteGUI(rendu,"1 A 9 : REPONSE",65,404,2,discretGUI);
    texteGUI(rendu,"FLECHES : DEPLACER",65,436,2,discretGUI);
    texteGUI(rendu,"ECHAP : ABANDON",65,468,2,discretGUI);
    texteGUI(rendu,"REGLES",790,330,2,vertGUI);
    texteGUI(rendu,"1 A 9 SANS DOUBLON",790,372,2,discretGUI);
    texteGUI(rendu,"PAR LIGNE, COLONNE",790,404,2,discretGUI);
    texteGUI(rendu,"ET BLOC 3 X 3.",790,436,2,discretGUI);
    texteGUI(rendu,"UN FAUX CHIFFRE",790,484,2,discretGUI);
    texteGUI(rendu,"COUTE UNE ERREUR.",790,516,2,discretGUI);
    texteGUI(rendu,"CASE FIXE : REFUS",790,564,2,discretGUI);
    texteGUI(rendu,"SANS PENALITE.",790,596,2,discretGUI);
    boutonGUI(rendu,"1  FACILE",365,320,370,62,sourisX,sourisY,0);
    texteGUI(rendu,"45 INDICES / 9 ERREURS",397,394,2,discretGUI);
    boutonGUI(rendu,"2  MOYEN",365,438,370,62,sourisX,sourisY,0);
    texteGUI(rendu,"35 INDICES / 5 ERREURS",397,512,2,discretGUI);
    boutonGUI(rendu,"3  DIFFICILE",365,556,370,62,sourisX,sourisY,0);
    texteGUI(rendu,"28 INDICES / 3 ERREURS",397,630,2,discretGUI);
    texteGUI(rendu,message,70,692,2,rougeGUI);
    texteGUI(rendu,"CLIC OU TOUCHES 1 / 2 / 3          ECHAP : QUITTER",222,727,2,discretGUI);
}

void grilleGUI(SDL_Renderer *rendu, int grille[TAILLE][TAILLE],
               int initiale[TAILLE][TAILLE], int selectionL, int selectionC,
               int fin, int erreurL, int erreurC, int erreurVisible)
{
    SDL_Color surligne = {238,245,233,255};
    SDL_Color choisi = {193,219,179,255};
    SDL_Color faute = {249,213,207,255};
    SDL_Color ligneFine = {189,201,185,255};
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            SDL_Color couleur = blancGUI;
            if (!fin && (i == selectionL || j == selectionC ||
                (i / N == selectionL / N && j / N == selectionC / N))) couleur = surligne;
            if (!fin && i == selectionL && j == selectionC) couleur = choisi;
            if (erreurVisible && i == erreurL && j == erreurC) couleur = faute;
            rectangleGUI(rendu,GRILLE_X+j*CASE_GUI,GRILLE_Y+i*CASE_GUI,CASE_GUI,CASE_GUI,couleur);
            if (grille[i][j] != 0) {
                SDL_Color chiffre = vertGUI;
                if (initiale[i][j] != 0) chiffre = encreGUI;
                nombreGUI(rendu,grille[i][j],GRILLE_X+j*CASE_GUI+20,GRILLE_Y+i*CASE_GUI+16,4,chiffre);
            }
        }
    }
    for (int i = 0; i <= TAILLE; i++) {
        int epaisseur = 1;
        SDL_Color couleur = ligneFine;
        if (i % N == 0) { epaisseur = 3; couleur = encreGUI; }
        rectangleGUI(rendu,GRILLE_X-1,GRILLE_Y+i*CASE_GUI-1,TAILLE*CASE_GUI+3,epaisseur,couleur);
        rectangleGUI(rendu,GRILLE_X+i*CASE_GUI-1,GRILLE_Y-1,epaisseur,TAILLE*CASE_GUI+3,couleur);
    }
    texteGUI(rendu,"CHIFFRES FONCES : INDICES FIXES",GRILLE_X,704,2,discretGUI);
    if (fin) texteGUI(rendu,"VERT : VALEURS COMPLETEES",GRILLE_X,730,2,vertGUI);
    else texteGUI(rendu,"CHIFFRES VERTS : REPONSES",GRILLE_X,730,2,vertGUI);
}

void partieGUI(SDL_Renderer *rendu, int grille[TAILLE][TAILLE],
               int initiale[TAILLE][TAILLE], int selectionL, int selectionC,
               int niveau, int erreurs, int limite, int etape, int sourisX, int sourisY,
               const char message[], int erreurL, int erreurC, int erreurVisible)
{
    const char *niveaux[] = {"FACILE","MOYEN","DIFFICILE"};
    texteGUI(rendu,"SUDOKU",50,38,8,encreGUI);
    texteGUI(rendu,niveaux[niveau-1],675,49,4,vertGUI);
    texteGUI(rendu,"ERREURS",675,103,2,discretGUI);
    nombreGUI(rendu,erreurs,785,103,2,rougeGUI);
    texteGUI(rendu,"/",808,103,2,discretGUI);
    nombreGUI(rendu,limite,832,103,2,discretGUI);
    grilleGUI(rendu,grille,initiale,selectionL,selectionC,etape!=1,erreurL,erreurC,erreurVisible);
    if (etape == 1) {
        texteGUI(rendu,"CASE",675,166,3,encreGUI);
        nombreGUI(rendu,selectionL+1,771,166,3,encreGUI);
        texteGUI(rendu,"/",801,166,3,encreGUI);
        nombreGUI(rendu,selectionC+1,831,166,3,encreGUI);
        texteGUI(rendu,"CHOISIR UN CHIFFRE",675,216,2,discretGUI);
        for (int v = 1; v <= 9; v++) {
            char chiffre[2] = {(char)('0'+v),'\0'};
            boutonGUI(rendu,chiffre,675+((v-1)%3)*115,250+((v-1)/3)*78,100,64,sourisX,sourisY,0);
        }
        texteGUI(rendu,"CLIC OU FLECHES : CASE",675,510,2,discretGUI);
        texteGUI(rendu,"TOUCHES 1 A 9 : CHIFFRE",675,539,2,discretGUI);
        texteGUI(rendu,message,675,585,2,encreGUI);
        boutonGUI(rendu,"ABANDONNER",675,642,330,55,sourisX,sourisY,0);
    } else {
        const char *titre = "PARTIE ABANDONNEE";
        if (etape == 2) titre = "VICTOIRE !";
        if (etape == 3) titre = "LIMITE ATTEINTE";
        texteGUI(rendu,titre,675,185,3,encreGUI);
        if (etape == 2) {
            texteGUI(rendu,"GRILLE COMPLETE",675,260,3,vertGUI);
            texteGUI(rendu,"ET VALIDE.",675,300,3,vertGUI);
        } else {
            texteGUI(rendu,"VOICI LA SOLUTION",675,260,3,vertGUI);
            texteGUI(rendu,"DE VOTRE GRILLE.",675,300,3,vertGUI);
        }
        texteGUI(rendu,"INDICES AU DEPART",675,375,2,discretGUI);
        nombreGUI(rendu,compterCasesInitiales(initiale),675,410,5,encreGUI);
        boutonGUI(rendu,"RETOUR AU MENU",675,540,330,60,sourisX,sourisY,1);
        boutonGUI(rendu,"QUITTER",675,622,330,55,sourisX,sourisY,0);
    }
}

void confirmationGUI(SDL_Renderer *rendu, int sourisX, int sourisY)
{
    SDL_Color ombre = {20,35,26,190};
    rectangleGUI(rendu,0,0,LARGEUR_GUI,HAUTEUR_GUI,ombre);
    rectangleGUI(rendu,235,255,630,245,blancGUI);
    texteGUI(rendu,"ABANDONNER LA PARTIE ?",300,290,3,encreGUI);
    texteGUI(rendu,"LA SOLUTION SERA AFFICHEE.",350,345,2,discretGUI);
    boutonGUI(rendu,"OUI / O",280,405,225,55,sourisX,sourisY,0);
    boutonGUI(rendu,"NON / N",590,405,225,55,sourisX,sourisY,1);
}

/* Les scancodes reconnaissent la rangee numerique meme sur un clavier AZERTY.
   Les touches du pave numerique sont egalement acceptees. */
int chiffreToucheGUI(SDL_KeyboardEvent touche)
{
    if (touche.keysym.scancode >= SDL_SCANCODE_1 && touche.keysym.scancode <= SDL_SCANCODE_9)
        return touche.keysym.scancode - SDL_SCANCODE_1 + 1;
    if (touche.keysym.sym >= SDLK_KP_1 && touche.keysym.sym <= SDLK_KP_9)
        return touche.keysym.sym - SDLK_KP_1 + 1;
    return 0;
}

int lancerInterfaceSDL(void)
{
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr,"SDL_Init : %s\n",SDL_GetError());
        return 1;
    }
    SDL_Window *fenetre = SDL_CreateWindow("Sudoku - Projet ASD",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,LARGEUR_GUI,HAUTEUR_GUI,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (fenetre == NULL) {
        fprintf(stderr,"SDL_CreateWindow : %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_SetWindowMinimumSize(fenetre,770,532);
    SDL_Renderer *rendu = SDL_CreateRenderer(fenetre,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (rendu == NULL) rendu = SDL_CreateRenderer(fenetre,-1,SDL_RENDERER_SOFTWARE);
    if (rendu == NULL) {
        fprintf(stderr,"SDL_CreateRenderer : %s\n",SDL_GetError());
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return 1;
    }
    /* SDL adapte aussi les coordonnees des clics a cette resolution logique. */
    if (SDL_RenderSetLogicalSize(rendu,LARGEUR_GUI,HAUTEUR_GUI) != 0) {
        fprintf(stderr,"SDL_RenderSetLogicalSize : %s\n",SDL_GetError());
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderDrawBlendMode(rendu,SDL_BLENDMODE_BLEND);
    int solution[TAILLE][TAILLE], grille[TAILLE][TAILLE], initiale[TAILLE][TAILLE];
    int niveau = 1, erreurs = 0, selectionL = 0, selectionC = 0;
    int sourisX = 0, sourisY = 0, quitter = 0, confirmation = 0;
    /* 0 : menu ; 1 : partie ; 2 : victoire ; 3 : defaite ; 4 : abandon. */
    int etape = 0, erreurL = -1, erreurC = -1;
    Uint32 momentErreur = 0;
    const char *message = "";
    SDL_Event evenement;

    while (!quitter) {
        while (SDL_PollEvent(&evenement)) {
            int chiffre = 0, choixNiveau = 0, clic = 0;
            if (evenement.type == SDL_QUIT) {
                if (etape == 1) confirmation = 1;
                else quitter = 1;
                continue;
            }
            if (evenement.type == SDL_MOUSEMOTION) {
                sourisX = evenement.motion.x;
                sourisY = evenement.motion.y;
            }
            if (evenement.type == SDL_MOUSEBUTTONDOWN && evenement.button.button == SDL_BUTTON_LEFT) {
                sourisX = evenement.button.x;
                sourisY = evenement.button.y;
                clic = 1;
            }
            if (confirmation) {
                int oui = clic && dansZoneGUI(sourisX,sourisY,280,405,225,55);
                int non = clic && dansZoneGUI(sourisX,sourisY,590,405,225,55);
                if (evenement.type == SDL_KEYDOWN && evenement.key.repeat == 0) {
                    oui = evenement.key.keysym.sym == SDLK_o;
                    non = evenement.key.keysym.sym == SDLK_n || evenement.key.keysym.sym == SDLK_ESCAPE;
                }
                if (oui) {
                    copierGrille(grille,solution);
                    etape = 4;
                    confirmation = 0;
                    erreurL = -1;
                }
                if (non) confirmation = 0;
                continue;
            }
            if (evenement.type == SDL_KEYDOWN && evenement.key.repeat == 0) {
                chiffre = chiffreToucheGUI(evenement.key);
                if (evenement.key.keysym.sym == SDLK_ESCAPE) {
                    if (etape == 1) confirmation = 1;
                    else if (etape == 0) quitter = 1;
                    else { etape = 0; message = ""; }
                }
            }
            if (etape == 0) {
                if (chiffre >= 1 && chiffre <= 3) choixNiveau = chiffre;
                if (clic) {
                    for (int i = 0; i < 3; i++)
                        if (dansZoneGUI(sourisX,sourisY,365,320+i*118,370,62)) choixNiveau = i+1;
                }
                if (choixNiveau != 0) {
                    SDL_SetRenderDrawColor(rendu,fondGUI.r,fondGUI.g,fondGUI.b,255);
                    SDL_RenderClear(rendu);
                    texteGUI(rendu,"GENERATION EN COURS...",352,355,3,vertGUI);
                    SDL_RenderPresent(rendu);
                    niveau = choixNiveau;
                    if (preparerPartie(solution,grille,initiale,niveau)) {
                        erreurs = 0; etape = 1; erreurL = -1;
                        selectionL = 0; selectionC = 0;
                        /* Selectionner la premiere case jouable. */
                        int trouve = 0;
                        for (int i = 0; i < TAILLE && !trouve; i++) {
                            for (int j = 0; j < TAILLE && !trouve; j++) {
                                if (initiale[i][j] == 0) { selectionL=i; selectionC=j; trouve=1; }
                            }
                        }
                        message = "A VOUS DE JOUER.";
                    } else message = "GENERATION IMPOSSIBLE. REESSAYEZ.";
                }
            } else if (etape == 1) {
                if (clic && dansZoneGUI(sourisX,sourisY,GRILLE_X,GRILLE_Y,TAILLE*CASE_GUI,TAILLE*CASE_GUI)) {
                    selectionL = (sourisY-GRILLE_Y)/CASE_GUI;
                    selectionC = (sourisX-GRILLE_X)/CASE_GUI;
                    message = "CHOISISSEZ UN CHIFFRE.";
                }
                if (evenement.type == SDL_KEYDOWN) {
                    if (evenement.key.keysym.sym == SDLK_LEFT && selectionC > 0) selectionC--;
                    if (evenement.key.keysym.sym == SDLK_RIGHT && selectionC < TAILLE-1) selectionC++;
                    if (evenement.key.keysym.sym == SDLK_UP && selectionL > 0) selectionL--;
                    if (evenement.key.keysym.sym == SDLK_DOWN && selectionL < TAILLE-1) selectionL++;
                }
                if (clic) {
                    for (int v = 1; v <= 9; v++)
                        if (dansZoneGUI(sourisX,sourisY,675+((v-1)%3)*115,250+((v-1)/3)*78,100,64)) chiffre=v;
                    if (dansZoneGUI(sourisX,sourisY,675,642,330,55)) confirmation=1;
                }
                if (chiffre != 0 && !confirmation) {
                    int resultat = jouerCoup(grille,initiale,solution,selectionL,selectionC,chiffre);
                    if (resultat == -2) message = "CASE INITIALE PROTEGEE.";
                    if (resultat == 0) {
                        erreurs++;
                        message = "CHIFFRE INCORRECT.";
                        erreurL=selectionL; erreurC=selectionC; momentErreur=SDL_GetTicks();
                        if (erreurs >= limiteErreurs(niveau)) {
                            etape=3;
                            copierGrille(grille,solution);
                            erreurL=-1;
                        }
                    }
                    if (resultat == 1) {
                        message = "BONNE REPONSE !";
                        if (grilleComplete(grille)) { etape=2; erreurL=-1; }
                    }
                }
            } else {
                if ((clic && dansZoneGUI(sourisX,sourisY,675,540,330,60)) ||
                    (evenement.type == SDL_KEYDOWN && evenement.key.keysym.sym == SDLK_RETURN)) {
                    etape=0; message="";
                }
                if (clic && dansZoneGUI(sourisX,sourisY,675,622,330,55)) quitter=1;
            }
        }
        SDL_SetRenderDrawColor(rendu,fondGUI.r,fondGUI.g,fondGUI.b,255);
        SDL_RenderClear(rendu);
        if (etape == 0) menuGUI(rendu,sourisX,sourisY,message);
        else partieGUI(rendu,grille,initiale,selectionL,selectionC,niveau,erreurs,
                       limiteErreurs(niveau),etape,sourisX,sourisY,message,erreurL,erreurC,
                       erreurL >= 0 && SDL_GetTicks()-momentErreur < 700);
        if (confirmation) confirmationGUI(rendu,sourisX,sourisY);
        SDL_RenderPresent(rendu);
        SDL_Delay(10);
    }
    SDL_DestroyRenderer(rendu);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    return 0;
}
