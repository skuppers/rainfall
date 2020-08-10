## Level 7

En dessasemblant le binaire `level7`, on observe quatres allocations de 8 bytes,
deux appel a `strcpy()`, un appel a `fopen()`, un appel a `fgets()` et un appel
a `puts()`.

Les deux appel a `strcpy()` copie respectivement le premier et le second
argument dans les allocations précédentes.

L'appel a `fopen()` ouvre le fichier `/home/user/level8/.pass`.

L'appel a `fgets()` lit le fichier, et stocke les bytes lu dans la globale `c`.

L'appel a `puts()` affiche le contenu de l'adresse `0x08048703`, qui contient
`~~'.

![image](https://user-images.githubusercontent.com/29956389/89811135-825fe780-db3e-11ea-9ad1-f7553e77d305.png)

Dans gdb le bit suid est annulé, le programme crash donc systématiquement quand
il arrive a l'appel de `fopen()`. On copie donc le binaire vers `/tmp/level7/lvl7`, et on crée un fichier `.pass` qui contient la string `Got the flag`, également dans ce dossier.

On poursuit donc le debugging, et on localise la string
`/home/user/level8/.pass` a l'adresse `0x080486eb`, et on la remplace par le
chemin vers notre fichier:

`gdb> set {char [18]} 0x080486eb = "/tmp/level7/.pass"`

Maintenant que le programme ne crash plus avec l'appel a `fopen()`, on se penche
sur le fonctionnement des `malloc()` et des `strcpy()`:

On place les breakpoints autour du premier `strcpy()` puis du second. On lance
ensuite la commande `r "AAAABBBBCCCCDDDDEEEEFFFF" "GGGGHHHHIIIIJJJJKKKKLLLL"`.

Le premier `strcpy()` ne crash pas, par contre on obtient un `SIGSEGV` pour le
second. On relance le programme et on s'arette juste avant l'appel au second.
On affiche les parametres placé sur la stack:

![image](https://user-images.githubusercontent.com/29956389/89812447-74ab6180-db40-11ea-9be4-6f2db32cf4e0.png)

Et on observe plusieurs choses: ESP vaut `0x46464646` ce qui correspond a
`FFFF`, ce qui veut dire que le premier appel a `strcpy()` écrase la destination
du second appel avec les bytes `20 a 24 (FFFF ici)`. Le second appel essaye donc
de copier notre deuxieme argument a l'adresse `0x46464646`.

Le but étant de lancer la fonction `m()` après avoir chargé le fichier `.pass`,
ce qui nous laisse uniquement le choix de 'remplacer' la fonction `puts()` par
la fonction `m()`.

La fonction `puts()` fait partie de la libc, lors de la compilation le linker va
remplir les sections `.got` et `.plt` du binaire pour y insérer les adresses de
ces fonctions. Comme par hasard, le binaire est compilé sans `RELRO (Relocation
Read-Only)`, ce qui veut dire que nous avons la possibilité de réécrire les
données de ces sections, pour y remplacer, par exemple, l'adresse de `puts()`
par l'adresse de `m()`.

Déssasemblons a présent, l'appel a `puts()`:

![image](https://user-images.githubusercontent.com/29956389/89813576-2f882f00-db42-11ea-87a8-1e902b62b6d0.png)

On observe un `jmp DWORD PTR ds:0x8049928`, l'adresse de `puts()` dans la
section `.got`.

![image](https://user-images.githubusercontent.com/29956389/89813735-6a8a6280-db42-11ea-9293-87bbd7924eca.png)

On connais a présent, l'adresse a remplacer, maintenant cherchons l'adresse de
`m()` avec `info func` ou directement dans cutter.

![image](https://user-images.githubusercontent.com/29956389/89814255-25b2fb80-db43-11ea-87c2-6b880055c3db.png)

On obtient: `0x080484f4`.

Avec tout les informations en main, passons a l'exploitation (dans gdb):

![image](https://user-images.githubusercontent.com/29956389/89814901-2c8e3e00-db44-11ea-9950-33db0e303e43.png)

Et ca marche !

On répète donc l'opération avec le vrai binaire, en dehors de GDB:

`$ ./level7 $(python -c 'print "AAAABBBBCCCCDDDDEEEE\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')

Et on obtient le flag !




