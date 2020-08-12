## Bonus3

En désassemblant le binaire, on observe une appel a `fopen("/home/user/end/.pass", 'r')`, deux appel a `fread()`, un appel a `strcmp()` et un appel a `system("/bin/sh")`. Le binaire prend un argument en entrée.

Comme GDB change les permissions du binaire, on va le copier dans le dossier
`/tmp` et remplacer la string `"/home/user/end/.pass"` de l'appel `fopen()` par
un fichier également dans le dossier `/tmp`. Il contient la string `Got the
final flag!!!`.

Le binaire charger le contenu du fichier dans un buffer, et va faire un appel a `atoi()` sur notre premier argument. Cette variable va etre utilisé a l'intruction juste après:

`*(undefined *)((int32_t)auStack152 + iVar2) = 0;`

Soit:

`buffer_qui_contient_le_flag[retour_de_atoi] = 0;`

Cette instruction va donc réécrire le buffer a l'index `retour_de_atoi` avec la
valeur `0`, qui vaut également `\0`. Deux instructions plus loin, on observe:

`iVar2 = strcmp(auStack152, envp[1]);`

`if (iVar2 == 0)
    execl("/bin/sh", 0x8048707, 0);`

Il faut donc faire en sorte que le retour de l'appel a `strcmp()` vaut 0. Soit
que notre argument soit égal au buffer.

Sans contourner cetter vérification, il faudrait que notre entrée soit égale au
contenue du buffer (Le flag). Mais si nous mettons `0` comment argument au
binaire, l'index `0` du buffer sera réécrit.

D'autre part, `atoi()` renvoie `0` quand l'argument qu'on lui passe n'est pas
valide. On peut donc envoyer une string vide au binaire, qui après l'appel a
`atoi()` sera également `0`. On se retrouve donc avec un appel a `strcmp("\0",
"\0")`.

La condition est vrai, est l'éxecution continue:

![image](https://user-images.githubusercontent.com/29956389/90034657-312c3100-dcc1-11ea-900d-426218e92b4b.png)

Et nous voila avec l'euid: `end`.


