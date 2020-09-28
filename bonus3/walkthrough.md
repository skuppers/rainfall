## Bonus3

En désassemblant le binaire, on observe une appel a `fopen("/home/user/end/.pass", 'r')`, deux appel a `fread()`, un appel a `strcmp()`, un a `atoi()` et un appel a `system("/bin/sh")`. 

Nous allons dans un premier temps remplir un fichier dans '/tmp/dummy' avec des
caracteres distinctifs pour voir correctement le comportement du programme avec
la commande suivante:

'python -c 'for i in "ABCDEFGHIJKLMNJKLMNOPQRSTUVWXYZ" : print i*4,' | tr -d ' ' > /tmp/dummy'

Le binaire a deux variables:
    - un pointeur sur FILE * pour stocker le retour de fopen.
    - une array de 128 caracteres que nous nomerons data.

Le binaire ouvre le fichier passe predefini avec fopen. Pour les besoins de
notre exploit, nous remplacons la string indiquant le fichier a ouvrir par notre
fichier personnel avec la commande suivante juste avant que le parametre soit
inscrit dans la stack :

set $eax = "/tmp/fakepass"

Apres l'appel a fopen, le binaire fait deux controle: le premier pour determiner
si le fopen a bien reussi, le second pour determiner si ac est egal a 2. il faut
donc passer un parametre en ligne de commande.

Si ces controles passent, un premier read de 66 caracteres sera effectue dans le
buffer data. Puis, `0` sera ecrit a l'index 65 de l'array data.

Une seconde ecriture de `0` sera effectue a l'index donne par le retour de
atoi(av[1]).

Apres quoi, un second fread de 65 caracteres sera effectue, a partir de
data[66].

Enfin, un strcmp entre av[1] et data est effectue, et si les deux sont
identiques, un shell nous est donne.

Il faut donc faire en sorte que le retour de l'appel a `strcmp()` retourne 0. Soit
que notre argument soit égal au buffer.

Sans contourner cetter vérification, il faudrait que notre entrée soit égale au
contenue du buffer (Le flag). Mais si nous mettons le caracter `0` commen argument au binaire, l'index 32 du buffer sera réécrit.

L'astuce ici est que `atoi()` renvoie `0` quand l'argument qu'on lui passe n'est pas
valide. On peut donc envoyer une string vide au binaire, qui après l'appel a
`atoi()` sera également `0`. On se retrouve donc avec un appel a `strcmp("\0",
"\0")`.

La condition est vrai, est l'éxecution continue:

![image](https://user-images.githubusercontent.com/29956389/90034657-312c3100-dcc1-11ea-900d-426218e92b4b.png)

Et nous voila avec l'euid: `end`.
