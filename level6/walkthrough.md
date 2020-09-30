## Level 6

![image](https://user-images.githubusercontent.com/29956389/94688735-4838e800-032e-11eb-9217-2543a2e9de02.png)

En dessasemblant le code, on observe une premiere allocation de `0x40 (64)` bytes,
pour stocker l'entrée de l'utilisateur.
Ensuite on observe une deuxieme allocation, un tableau, ou la premiere adresse
pointe sur la fonction m().

Cette fonction n'est pas utile, contrairement a la fonction n(), qui execute `system("/bin/cat /home/user/level7/.pass")`.

Ensuite l'entrée utilisateur est copié dans le premier buffer alloué.
Les deux allocation sont éffectué l'une après l'autre, il forme un espace
memoire continue, sur lequel nous avons les droits d'écriture.

Le but est donc de remplacer le pointeur de fonction m() par n().

On localise donc l'adresse de n(): `0x08048454`.

Sachant que l'espace mémoire est continue, il nous suffit donc de remplir les
premier 64 bytes puis de remplir le tableau de pointeur de fonction par l'adresse de n().

`$ ./level6 "[64 bytes de remplissage] + [Trois fois l'adresse de n()]"`

`$ ./level6 $(python -c 'print "B" *64 + "\x54\x84\x04\x08"*4')`

Et le tour est joué.
