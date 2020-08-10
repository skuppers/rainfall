## Level 4

Comme au niveau précédent, nous décompilons le binaire.

Appart le main, on retrouve deux fonctions p() et n().

n() fonctionne comme a l'exercice d'avant, il faut donc a nouveau exploiter les
formats strings.

p() fait juste un appel a printf();

Par contre, la valeur a réécrite dans '_m' vaut 0x1025544 soit 16 930 116 en
décimal. Cette variable se situe a l'adresse 0x08049810.

On commence par trouver l'offset de notre argument dans la stack:

$ ./level4
> BBBB %x %x %x %x %x %x %x %x %x %x %x %x

On observe que la valeur 0x42424242 (BBBB) est décalé de 12, due a l'appel de
fonction p() entre le fgets() et le printf(). On sait donc qu'il faudra utiliser
'%12$n'.

Avec toutes ces informations, on construit donc la commande:
$ python -c 'print "\x10\x98\x04\x08" + "B"*16930112 + "%12$n"' | ./level4

Sauf que écrire plus de 16 millions de 'B' ne fonctionne évidement pas. (Cela
serait trop facile)

On va donc allez un pas plus loin dans l'exploitation des format string, en
utilisant la taille de champ de printf(). Ce qui nous donne :

$ python -c 'print "\x10\x98\x04\x08" + "%16930112d" + "%12$n"' | ./level4

Ce qui nous permet de ne pas afficher ces 16 millions de charactères, mais qui
seront quand meme insérér par printf() lors de la résolution du format.

Pas besoin cette fois ci d'ecrire le retour de la fonction dans un fichier, car
la commande executé par system() n'est pas un shell mais, 'cat
/home/user/level5/.pass'.

Le flag est donc directement affiché sur notre terminal.
