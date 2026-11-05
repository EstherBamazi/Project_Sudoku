#include "joueur.h"
#include <stdio.h>
#include <assert.h>

int main(void)
{
    int valeurs[3];
    char texte[128];
    assert(lireEntiers("1 2 3", valeurs, 3) == 1);
    assert(valeurs[0] == 1 && valeurs[1] == 2 && valeurs[2] == 3);
    assert(lireEntiers(" \t1\t2  3 \r", valeurs, 3) == 1);
    char *refus[] = {"", " ", "1", "1 2", "1 2 3 4", "123", "0 2 3",
                    "10 2 3", "1x 2 3", "+1 2 3", "01 2 3", "1.0 2 3"};
    for (int i = 0; i < 12; i++) assert(lireEntiers(refus[i], valeurs, 3) == 0);
    assert(commande(" Q \r", 'q', 'Q') == 1);
    assert(commande("q extra", 'q', 'Q') == 0);
    assert(commande("qq", 'q', 'Q') == 0);
    assert(commande("", 'q', 'Q') == 0);
    assert(commande(" O ", 'o', 'O') == 1);
    int lecture;
    while ((lecture = lireLigne(texte, sizeof texte)) != 0) {
        printf("%d %d %d\n", lecture, lireEntiers(texte, valeurs, 3), commande(texte, 'q', 'Q'));
    }
    return 0;
}
