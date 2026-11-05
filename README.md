# Sudoku en C

Jeu 9 × 9 en console, avec génération et résolution par backtracking classique.

## Fichiers et compilation

- `main.c` : saisies, niveau et déroulement de la partie.
- `Sudoku.c` : validation, résolution, génération, comptage et affichage.
- `Sudoku.h` : constantes et déclarations des fonctions du moteur.
- `CMakeLists.txt` : compilation de la cible `sudoku`.
- `partie.c` et `partie.h` : contribution de gestion de partie ajoutée par Cédric.
- `main Cedric.c` : autre version du programme principal, à intégrer séparément.

La cible actuelle compile uniquement `main.c` et `Sudoku.c`. Les trois fichiers
de Cédric sont conservés mais ne sont pas encore utilisés par cette cible.
`main Cedric.c` contient toujours l'ancienne saisie avec `strtol` ; la saisie
simplifiée décrite ci-dessous concerne le `main.c` actuellement compilé.
Ne pas compiler les deux fichiers `main` dans le même exécutable.

Ouvrir la racine du dépôt dans CLion comme projet CMake, puis lancer `sudoku`.
Le dépôt ne contient pas de sous-dossier `moteur`, de fichier `test_sudoku.c`,
ni de cible de tests. CLion place ses fichiers générés dans `cmake-build-debug`.

Avec CMake installé, depuis la racine :

```text
cmake -S . -B build
cmake --build build
```

Lancer l'exécutable produit (par exemple `build/sudoku.exe` avec MinGW sous
Windows, ou `./build/sudoku` sous Linux).

## Jouer

Choisir 1, 2 ou 3. Pour jouer, écrire **trois chiffres sur une seule ligne**,
séparés par des espaces, puis appuyer sur Entrée :

```text
2 4 7
```

Cela place 7 à la ligne 2, colonne 4. Les coordonnées saisies vont de 1 à 9 ;
les indices internes vont de 0 à 8. Un point représente une case vide.

Chaque nombre doit être un seul chiffre : `01`, `+1`, `123` et `1.0`
sont refusés. Les espaces et tabulations sont acceptés.
Une saisie mal formée ou trop longue est refusée sans pénalité.

Taper `q`, puis `o`, pour abandonner. Les majuscules sont acceptées.
Une réponse différente de `o` annule la demande d'abandon.

| Niveau | Indices | Minimum par ligne, colonne et bloc | Erreurs maximales |
| --- | ---: | ---: | ---: |
| Facile | 45 | 4 | 9 |
| Moyen | 35 | 3 | 5 |
| Difficile | 28 | 2 | 3 |

Un faux chiffre est refusé et compte comme une erreur. Une case initiale est
protégée : tenter de la modifier ne coûte pas d'erreur.
La victoire correspond à une grille complète et valide.

## Comprendre le moteur

Les tableaux sont indépendants : `solution` conserve la réponse complète,
`initiale` les cases protégées, et `grille` la partie en cours. 0 signifie vide.

- `placementPossible` vérifie ligne, colonne et bloc sans modifier la grille.
  La case examinée elle-même est ignorée pour pouvoir vérifier une case remplie.
- `choisirCase` sélectionne la première case vide et prépare ses candidats.
  Les pointeurs `ligne` et `colonne` transmettent les coordonnées à l'appelant.
- `resoudre` place un candidat, s'appelle récursivement et annule si la suite
  échoue. En cas de succès, elle conserve la solution.
- `resoudreGrille` valide d'abord la grille fournie, puis appelle `resoudre`.
- `compter` explore de la même manière, mais annule chaque placement, même
  après une solution, pour poursuivre la recherche.
- `compterSolutions` lance le comptage avec une limite de 2 : 0 signifie aucune
  solution, 1 une solution unique, 2 au moins deux solutions.
- `genererPartie` construit une solution, la copie et essaie de retirer des
  valeurs. Chaque retrait doit préserver les minima et l'unicité.
- `jouerCoup` protège les cases initiales et compare le chiffre à la solution.
- `initialiserGrille`, `copierGrille`, `grilleValide`, `grilleComplete`
  et `afficherGrille` gèrent les opérations courantes sur la matrice.

### Pourquoi limite - total ?

Si l'on cherche au maximum 2 solutions et qu'on en a déjà trouvé 1, il n'en
reste qu'une à chercher. La variable `restantes` vaut donc `limite - total`.
Cela évite de dépasser la limite et de poursuivre inutilement la recherche.

### Pourquoi le comptage ne fait-il pas de copie ?

Chaque appel choisit une case vide, place un candidat, explore, puis remet
cette case à 0. Les appels plus profonds annulent aussi leurs propres essais.
Tous les placements temporaires sont donc annulés avant le retour à l'appelant.
C ne restaure pas un tableau automatiquement : c'est l'affectation à 0 qui le fait.

## Notions du cours et ajouts pour le projet

La saisie utilise `getchar`, un tableau de caractères, des indices et des boucles.
Un chiffre est converti avec `texte[position] - '0'`.
Dans les fichiers compilés par la cible `sudoku`, il n'y a plus de `strtol`,
`errno`, `fgets` ou de fonctions `static`.
Les fonctions auxiliaires sont définies avant leurs appels.

La récursion, les tableaux et le passage par adresse sont les notions du cours
utilisées par le backtracking.

Pour la génération aléatoire, trois fonctions standard sont conservées et doivent
être comprises, même si elles n'ont pas été retrouvées dans les supports examinés :
`rand` fournit un entier pseudo-aléatoire, `srand` initialise cette suite et
`time` permet de varier l'initialisation entre les lancements.

`melanger` utilise des échanges : pour la position i, tirer j entre 0 et i,
échanger les deux valeurs à l'aide d'une variable temporaire, puis passer à i - 1.
Cette méthode s'appelle Fisher-Yates ; elle est conservée et expliquée.

| Bibliothèque | Utilisation |
| --- | --- |
| `stdio.h` | `printf`, `puts`, `getchar`, `fflush`, `EOF` |
| `stdlib.h` | `rand`, `srand` |
| `time.h` | `time` |

Aucune bibliothèque Sudoku n'est utilisée. MRV n'est pas implémentée.

## Limites

Le jeu et les niveaux sont prévus pour le 9 × 9. Le nombre d'indices et les minima
ne mesurent pas à eux seuls la difficulté logique pour un joueur.
La génération effectue au maximum 100 tentatives, puis signale son échec.

