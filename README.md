# Sudoku en C

Jeu de Sudoku 9 × 9 avec génération et résolution par **backtracking classique**,
solution unique et trois niveaux. Le même moteur sert à l'interface console et
à l'interface graphique SDL2. Aucune bibliothèque spécialisée Sudoku n'est utilisée.

## Organisation du projet

| Fichiers | Rôle |
| --- | --- |
| `main.c` | Initialise l'aléatoire et choisit l'interface. C'est l'unique `main` du jeu. |
| `Sudoku.c`, `Sudoku.h` | Matrices, contraintes, résolution, génération, comptage et vérification des coups. L'affichage console de la grille est aussi dans ce module. |
| `partie.c`, `partie.h` | Préparation commune de la partie, comptage des indices et limite d'erreurs. |
| `joueur.c`, `joueur.h` | Lecture des saisies et boucle de jeu en console. |
| `interfaceSDL.c`, `interfaceSDL.h` | Fenêtre, dessin, souris, clavier et états de l'interface graphique. |
| `tests/` | Tests du moteur, de la préparation et de la saisie ; vérification de parties console. |
| `outils/preparer_sdl2.ps1` | Prépare la dépendance SDL2 pour MinGW 64 bits sous Windows. |
| `livrables/` | Documents du projet. |
| `CMakeLists.txt` | Configure la compilation du jeu et les tests facultatifs. |

Les apports de **Cédric** sont intégrés dans `partie.c` : `preparerPartie` génère
la partie puis copie les indices dans `initiale`, et `compterCasesInitiales`
compte les cases remplies. La séparation de la boucle console dans `joueur.c`
reprend l'apport de **Rayan**, avec l'affichage de la solution après abandon ou
défaite. Ces fonctions sont désormais appelées par le programme compilé.

Les anciennes copies `main Cedric.c`, `main rayan .c`, `joueur.c.c` et `joueur.h.c`
ont été retirées pour éviter plusieurs programmes principaux ou des fichiers
inutilisés. `.idea` est retiré du suivi Git. Les dossiers de compilation,
`third_party` et les exécutables sont ignorés par `.gitignore`.

## Compiler et lancer en console

La console est le mode de compilation par défaut ; elle ne nécessite pas SDL2.
Dans CLion, ouvrir la racine du dépôt comme projet CMake et lancer la cible `sudoku`.

Dans un terminal où **CMake et un compilateur C sont disponibles**, depuis la
racine du dépôt :

```powershell
cmake -S . -B build -DSUDOKU_AVEC_SDL=OFF
cmake --build build
.\build\sudoku.exe
```

L'exemple d'exécution correspond à Windows avec MinGW. Sous Linux, utiliser
`./build/sudoku`. Un générateur avec plusieurs configurations peut placer
l'exécutable dans un sous-dossier tel que `Debug`.

## Compiler et lancer avec SDL2

### Windows, CLion et MinGW 64 bits

Depuis la racine du dépôt, exécuter dans PowerShell :

```powershell
.\outils\preparer_sdl2.ps1
```

Le script télécharge **SDL2 2.32.10** depuis les publications officielles du projet
SDL, contrôle l'empreinte **SHA-256** attendue, puis extrait l'archive dans
`third_party`. Il n'installe rien dans les dossiers système et affiche le chemin
CMake à utiliser. Cette archive est destinée à **MinGW 64 bits**, pas à MSVC.

Dans les options CMake du profil CLion utilisant MinGW, ajouter les deux options
suivantes en remplaçant le début du chemin par celui de votre clone. Le script
fournit directement la ligne contenant votre chemin réel :

```text
-DSUDOKU_AVEC_SDL=ON -DSDL2_DIR="C:/chemin/vers/Project_Sudoku/third_party/SDL2-2.32.10/x86_64-w64-mingw32/lib/cmake/SDL2"
```

Recharger le projet CMake, puis lancer `sudoku`. La fenêtre graphique s'ouvre.
Avec cette configuration Windows, CMake copie `SDL2.dll` à côté de l'exécutable.

On peut aussi compiler depuis un terminal disposant de CMake et de la chaîne
MinGW. Un dossier distinct évite de réutiliser une ancienne configuration :

```powershell
$configurationSDL = (Resolve-Path '.\third_party\SDL2-2.32.10\x86_64-w64-mingw32\lib\cmake\SDL2').Path
cmake -S . -B build-sdl -DSUDOKU_AVEC_SDL=ON "-DSDL2_DIR=$configurationSDL"
cmake --build build-sdl
.\build-sdl\sudoku.exe
```

Le binaire graphique conserve la console. Pour la choisir au lancement :

```powershell
.\build-sdl\sudoku.exe --console
```

Dans CLion, `--console` se place dans les arguments du programme. Sur une autre
plateforme, fournir une installation SDL2 adaptée et son paquet CMake
`SDL2Config.cmake` ; le script PowerShell ci-dessus vise uniquement MinGW Windows.

## Jouer

Chaque ligne, colonne et bloc 3 × 3 doit contenir les chiffres de 1 à 9 sans doublon.

| Niveau | Indices au départ | Minimum par ligne, colonne et bloc | Erreurs maximales |
| --- | ---: | ---: | ---: |
| Facile | 45 | 4 | 9 |
| Moyen | 35 | 3 | 5 |
| Difficile | 28 | 2 | 3 |

Un chiffre différent de la solution est refusé et compte comme une erreur.
Les cases initiales sont protégées : tenter de les modifier ne coûte aucune erreur.
La victoire correspond à une grille complète et valide. Après défaite ou abandon
confirmé, la solution est affichée.

### Interface graphique

- Dans le menu, cliquer sur un niveau ou utiliser les touches `1`, `2` et `3`.
- Choisir une case par clic ou déplacer la sélection avec les flèches.
- Entrer une valeur avec les touches `1` à `9`, le pavé numérique ou les boutons.
- Les indices initiaux sont foncés ; les réponses acceptées sont vertes.
- Un chiffre incorrect est refusé, la case est brièvement signalée et le compteur augmente.
- Le bouton **Abandonner**, `Échap` ou la fermeture de la fenêtre pendant une partie
  ouvrent une confirmation. `O` confirme ; `N` ou `Échap` annulent.
- Après la fin d'une partie, **Retour au menu** ou `Entrée` permettent de rejouer.
  `Échap` revient aussi au menu ; **Quitter** ferme la fenêtre.
- Dans le menu, `Échap` ferme la fenêtre. La fenêtre peut être redimensionnée.

### Interface console

Choisir `1`, `2` ou `3`, puis écrire **trois chiffres sur une seule ligne**, séparés
par des espaces, avant d'appuyer sur Entrée :

```text
2 4 7
```

Cela place 7 à la ligne 2, colonne 4. Les coordonnées saisies vont de 1 à 9 ;
les indices internes vont de 0 à 8. Un point représente une case vide.

Chaque nombre doit être un seul chiffre : `01`, `+1`, `123` et `1.0` sont refusés.
Les espaces et tabulations sont acceptés. Une saisie mal formée ou trop longue
est refusée sans pénalité. Taper `q`, puis `o`, pour abandonner ; les majuscules
sont acceptées. Une réponse différente de `o` annule la demande d'abandon.

## Exécuter les tests

Les tests sont présents dans `tests/` et se compilent séparément du jeu.
Ils sont désactivés par défaut. Pour les activer :

```powershell
cmake -S . -B build -DBUILD_TESTING=ON -DSUDOKU_AVEC_SDL=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

Les trois tests C ne demandent pas Python :

- `test_moteur` génère **90 parties, 30 par niveau** avec des graines fixées et
  vérifie validité, unicité, nombre d'indices, minima, restauration et coups.
- `test_partie` vérifie la préparation des trois tableaux, leur indépendance,
  le comptage des cases initiales et les niveaux invalides.
- `test_saisie` vérifie le format des chiffres et des commandes.

Si Python 3 est trouvé par CMake, un quatrième test, `console`, utilise
`tests/verifier_console.py` pour vérifier les saisies longues, les fins de fichier,
les cases protégées, les erreurs, l'abandon et une victoire. Les mêmes tests
peuvent être activés avec `-DBUILD_TESTING=ON` dans le dossier `build-sdl` ; le test
console lance alors le binaire avec `--console`.

Avec un générateur à plusieurs configurations, ajouter la configuration à CTest,
par exemple `ctest --test-dir build -C Debug --output-on-failure`.
Les tests console ne remplacent pas une vérification visuelle des interactions SDL.

## Comprendre le moteur

Les tableaux sont indépendants : `solution` conserve la réponse complète,
`initiale` les cases protégées, et `grille` la partie en cours. **0 signifie vide.**

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
- `preparerPartie` lance la génération puis conserve les indices de départ.
- `limiteErreurs` fournit la limite commune aux deux interfaces.
- `initialiserGrille`, `copierGrille`, `grilleValide`, `grilleComplete`,
  `compterCasesInitiales` et `afficherGrille` réalisent les opérations courantes.

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

La saisie console utilise `getchar`, un tableau de caractères, des indices et des
boucles. Un chiffre est converti avec `texte[position] - '0'`. Les fichiers du jeu
n'utilisent plus `strtol`, `errno`, `fgets` ou de fonctions `static`.
La récursion, les tableaux et le passage par adresse servent au backtracking.
`const` indique les données que l'on ne souhaite pas modifier.

Pour l'aléatoire, trois fonctions standard sont conservées et doivent être
comprises, même si elles n'ont pas été retrouvées dans les supports examinés :
`rand` fournit un entier pseudo-aléatoire, `srand` initialise cette suite et
`time` permet de varier l'initialisation entre les lancements.

`melanger` utilise des échanges : pour la position i, tirer j entre 0 et i,
échanger les deux valeurs à l'aide d'une variable temporaire, puis passer à i - 1.
Cette méthode s'appelle Fisher-Yates ; elle est conservée et expliquée dans le code.

`main` reçoit `argc` et `argv` pour lire les arguments du lancement.
`strcmp`, fourni par `string.h`, permet de reconnaître `--console`.

SDL2 constitue une extension graphique autorisée par le sujet. Elle introduit
une boucle d'événements, des pointeurs vers les objets `SDL_Window` et
`SDL_Renderer`, et l'union `SDL_Event`. Le champ de cette union consulté dépend
du type d'événement : clavier, souris ou fenêtre. `SDL_RenderSetLogicalSize`
conserve un espace de dessin commun lorsque la fenêtre change de taille.
La petite police dessinée dans le code évite de dépendre de SDL_ttf et d'un
fichier de police externe.

| Bibliothèque | Utilisation |
| --- | --- |
| `stdio.h` | Affichage console, `getchar`, `fflush`, `EOF`, messages d'erreur |
| `stdlib.h` | `rand`, `srand` |
| `time.h` | `time` |
| `string.h` | Comparaison de l'argument `--console` avec `strcmp` |
| `SDL.h` | Fenêtre, rendu et événements, uniquement si SDL2 est activée |

MRV n'est pas implémentée. L'interface SDL2 réutilise le moteur existant ; elle ne
résout pas les grilles elle-même.

## Limites

Le jeu et les niveaux sont prévus pour le 9 × 9. Le nombre d'indices et les minima
ne mesurent pas à eux seuls la difficulté logique pour un joueur.
La génération effectue au maximum 100 tentatives, puis signale son échec.
Dans la fenêtre SDL, cette génération est synchrone : la partie commence après
la préparation de la grille. Des tests réussis ne prouvent pas tous les cas possibles.
